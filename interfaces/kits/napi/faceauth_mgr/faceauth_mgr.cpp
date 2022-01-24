/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "faceauth_mgr.h"

#include "faceauth_log_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "singleton.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace {
constexpr size_t ARGS_NUMBER_ONE = 1;
constexpr size_t ARGS_NUMBER_TWO = 2;
constexpr size_t ARGS_NUMBER_THREE = 3;
constexpr size_t MAX_REQ_ID_COUNT = 10000;
const std::string FACEAUTH_CLASS_NAME = "FaceAuthAbility";
} // namespace

Tips FaceauthMgr::tip_;
napi_env FaceAuthNativeEvent::env_;
napi_ref FaceauthMgr::constructorRef_;
FaceauthMgr *faceauthNapi_ = nullptr;
std::mutex FaceauthMgr::mutex_;
std::map<uint64_t, AsyncFaceAuthCallbackInfo *> FaceauthMgr::callbackMap_;

Tips::Tips() noexcept
{
    this->tipCode = 0;
    this->tipEvent = 0;
    this->errorCode = 0;
}

napi_value FaceauthMgr::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    FaceauthMgr *obj = new FaceauthMgr(env, thisVar);
    napi_wrap(
        env, thisVar, reinterpret_cast<void *>(obj),
        [](napi_env env, void *data, void *hint) {
            (void) env;
            (void) hint;
            FaceauthMgr *faceauthMgr = reinterpret_cast<FaceauthMgr *>(data);
            delete faceauthMgr;
        },
        nullptr, &(obj->wrapper_));
    return thisVar;
}

class FaceauthMgr::FaceAuthCallback : public OnFaceAuthStub {
public:
    FaceAuthCallback() = default;
    virtual ~FaceAuthCallback() = default;

    void OnCallbackEvent(uint64_t reqId, int32_t type, int32_t code, int32_t errorCode, Bundle &bundle)
    {
        FACEAUTH_LABEL_LOGI(
            "OnCallbackEvent for reqId: xxxx%04llu, type: %{public}d, code: %{public}d, errorCode: %{public}d", reqId,
            type, code, errorCode);
        napi_value tip;
        int32_t tipCode = 0;
        std::string tipInfo;
        napi_create_object(FaceAuthNativeEvent::env_, &tip);
        std::string errStr = std::to_string(code) + "+" + std::to_string(errorCode);
        FACEAUTH_LABEL_LOGI("errStr = %{public}s", errStr.c_str());
        if (Constant::TIP_CODE_MAP.count(errStr) > 0) {
            tipCode = Constant::TIP_CODE_MAP.at(errStr);
            FACEAUTH_LABEL_LOGI("tipCode = %{public}d", tipCode);
        }
        if (Constant::TIP_INFO_MAP.count(errStr) > 0) {
            tipInfo = Constant::TIP_INFO_MAP.at(errStr);
            FACEAUTH_LABEL_LOGI("tipInfo = %{public}s", tipInfo.c_str());
        }
        FaceauthMgr::SetValueInt32(FaceAuthNativeEvent::env_, "tipEvent", code, tip);
        FaceauthMgr::SetValueInt32(FaceAuthNativeEvent::env_, "errorCode", errStr == "0+0" ? 1 : 0, tip);
        FaceauthMgr::SetValueInt32(FaceAuthNativeEvent::env_, "tipCode", tipCode, tip);
        FaceauthMgr::SetValueString(FaceAuthNativeEvent::env_, "tipInfo", tipInfo, tip);
        if (faceauthNapi_ != nullptr) {
            faceauthNapi_->OnEvent("tip", ARGS_NUMBER_ONE, &tip);
        }
        if (code == Constant::CODE_ACQUIRE) {
            FaceauthMgr::tip_.tipEvent = code;
            FaceauthMgr::tip_.errorCode = errStr == "0+0" ? 1 : 0;
            FaceauthMgr::tip_.tipCode = tipCode;
            FaceauthMgr::tip_.tipInfo = tipInfo;
        }
        if (code == Constant::CODE_RESULT || type == Constant::TYPE_ENROLL) {
            auto iter = FaceauthMgr::callbackMap_.find(reqId);
            if (iter != FaceauthMgr::callbackMap_.end()) {
                FACEAUTH_LABEL_LOGI(" Find callback founction success!");
                iter->second->ret = tipCode;
                iter->second->code = code;
                iter->second->errorCode = errorCode;
                napi_status sta = napi_queue_async_work(iter->second->env, iter->second->asyncWork);
                FACEAUTH_LABEL_LOGI("async_work status:%{public}d", sta);
                iter->second = nullptr;
                FaceauthMgr::callbackMap_.erase(iter);
            }
            if (FaceAuthInnerKit::Release() == Constant::FAILURE) {
                FACEAUTH_LABEL_LOGE(" Release fail");
            }
        }
    }

    void OnDeathRecipient()
    {
        auto iter = FaceauthMgr::callbackMap_.begin();
        while (iter != FaceauthMgr::callbackMap_.end()) {
            FACEAUTH_LABEL_LOGI(" Death Recipient callback called!");
            FACEAUTH_LABEL_LOGI("callback reqId = xxxx%04llu", iter->first);
            if (iter->second) {
                iter->second->ret = ERRCODE_FAIL;
                iter->second->code = CODE_CALLBACK_RESULT;
                iter->second->errorCode = ERRCODE_FAIL;
                napi_status sta = napi_queue_async_work(iter->second->env, iter->second->asyncWork);
                FACEAUTH_LABEL_LOGI("async_work status:%{public}d", sta);
                iter->second = nullptr;
            }
            FaceauthMgr::callbackMap_.erase(iter++);
        }
    }
};

napi_value FaceauthMgr::Init(napi_env env, napi_value exports)
{
    FACEAUTH_LABEL_LOGI("faceauth napi begin init");
    OHOS::sptr<FaceauthMgr::FaceAuthCallback> callback(new FaceauthMgr::FaceAuthCallback());
    FaceAuthInnerKit::RegisterDeathRecipient(callback);
    std::cout << "faceauth napi begin init" << std::endl;
    napi_property_descriptor descriptor[] = {
        DECLARE_NAPI_FUNCTION("getAuthenticator", FaceauthMgr::GetAuthenticator),
        DECLARE_NAPI_FUNCTION("init", FaceauthMgr::JsInit),
        DECLARE_NAPI_FUNCTION("release", FaceauthMgr::Release),
        DECLARE_NAPI_FUNCTION("enroll", FaceauthMgr::Enroll),
        DECLARE_NAPI_FUNCTION("cancelEnroll", FaceauthMgr::CancelEnroll),
        DECLARE_NAPI_FUNCTION("preEnroll", FaceauthMgr::PreEnroll),
        DECLARE_NAPI_FUNCTION("postEnroll", FaceauthMgr::PostEnroll),
        DECLARE_NAPI_FUNCTION("remove", FaceauthMgr::Remove),
        DECLARE_NAPI_FUNCTION("getEnrolledFaceIDs", FaceauthMgr::GetEnrolledFaceIds),
        DECLARE_NAPI_FUNCTION("getAngleDim", FaceauthMgr::GetAngleDim),
    };
    napi_define_properties(env, exports, sizeof(descriptor) / sizeof(napi_property_descriptor), descriptor);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("execute", FaceauthMgr::Execute),
        DECLARE_NAPI_FUNCTION("cancel", FaceauthMgr::Cancel),
        DECLARE_NAPI_FUNCTION("checkAvailability", FaceauthMgr::CheckAvailability),
        DECLARE_NAPI_FUNCTION("getTips", FaceauthMgr::GetTips),
        DECLARE_NAPI_FUNCTION("on", FaceauthMgr::JsOn),
        DECLARE_NAPI_FUNCTION("off", FaceauthMgr::JsOff),
    };

    FACEAUTH_LABEL_LOGI("FaceauthMgr::Init() is called!");
    napi_value cons = nullptr;
    napi_define_class(env, FACEAUTH_CLASS_NAME.c_str(), FACEAUTH_CLASS_NAME.size(), JsConstructor, nullptr,
        sizeof(properties) / sizeof(napi_property_descriptor), properties, &cons);
    napi_create_reference(env, cons, 1, &constructorRef_);
    napi_set_named_property(env, exports, FACEAUTH_CLASS_NAME.c_str(), cons);
    return exports;
}

FaceauthMgr::FaceauthMgr(napi_env env, napi_value thisVar) : FaceAuthNativeEvent(env, thisVar), wrapper_(nullptr)
{
    env_ = env;
}

FaceauthMgr::~FaceauthMgr()
{
    if (wrapper_ != nullptr) {
        napi_delete_reference(env_, wrapper_);
    }
}

static int64_t GetTimestamp()
{
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &t);
    int64_t elapsedTime { ((t.tv_sec) * SEC_TO_NANOSEC + t.tv_nsec) };
    return elapsedTime;
}

void FaceauthMgr::InsertMap(int64_t reqId, AsyncFaceAuthCallbackInfo *asyncCallbackInfo)
{
    std::lock_guard<std::mutex> LockGuard(mutex_);
    while (FaceauthMgr::callbackMap_.count(reqId) > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        reqId = GetTimestamp() % MAX_REQ_ID_COUNT;
    }
    FaceauthMgr::callbackMap_.insert(
        std::map<uint64_t, AsyncFaceAuthCallbackInfo *>::value_type(reqId, asyncCallbackInfo));
}

void FaceauthMgr::SetValueInt32(const napi_env &env, const std::string &fieldStr, const int32_t intValue,
    napi_value &result)
{
    napi_value value;
    napi_create_int32(env, intValue, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

void FaceauthMgr::SetValueString(const napi_env &env, const std::string &fieldStr, const std::string &strValue,
    napi_value &result)
{
    napi_value value;
    napi_create_string_utf8(env, strValue.c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

static std::string GetStringFromNAPI(napi_env env, napi_value value)
{
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, 0, &size) != napi_ok) {
        FACEAUTH_LABEL_LOGE("can not get string size");
        return "";
    }
    result.reserve(size + 1);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + 1), &size) != napi_ok) {
        FACEAUTH_LABEL_LOGE("can not get string value");
        return "";
    }
    return result;
}

static napi_value ParseString(napi_env env, std::string &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    param = GetStringFromNAPI(env, args);
    FACEAUTH_LABEL_LOGI("param=%{public}s.", param.c_str());
    // create result code
    napi_value result;
    status = napi_create_int32(env, 1, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static napi_value ParseUint8(napi_env env, std::vector<uint8_t> &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    std::string str = GetStringFromNAPI(env, args);
    std::vector<uint8_t> unChar(str.begin(), str.end());
    unChar.push_back('\0');
    for (std::vector<uint8_t>::const_iterator iter = unChar.begin(); iter != unChar.end(); ++iter) {
        param.push_back(*iter);
    }
    // create result code
    napi_value result;
    status = napi_create_int32(env, 1, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static int32_t KitAuthentication(uint64_t reqId, std::string &type, std::string &level)
{
    FACEAUTH_LABEL_LOGI("type = %{public}s, level = %{public}s", type.c_str(), level.c_str());
    OHOS::sptr<FaceauthMgr::FaceAuthCallback> callback(new FaceauthMgr::FaceAuthCallback());
    AuthParam param;
    param.reqId = reqId;
    return UseriamAuthKit::Execute(param, type, level, callback);
}

static int32_t KitCancel(napi_env env)
{
    FACEAUTH_LABEL_LOGI("KitCancel");
    auto it = FaceauthMgr::callbackMap_.begin();
    auto itend = FaceauthMgr::callbackMap_.end();
    if (it != itend) {
        OHOS::sptr<FaceauthMgr::FaceAuthCallback> callback(new FaceauthMgr::FaceAuthCallback());
        uint64_t reqId = it->first;
        FACEAUTH_LABEL_LOGI("KitCancel reqId = xxxx%04llu", reqId);
        return UseriamAuthKit::Cancel(reqId, callback);
    }
    FACEAUTH_LABEL_LOGI("callbackMap is null");
    return Constant::FAILURE;
}

static int32_t KitCheckAvailability(napi_env env, std::string &type, std::string &level)
{
    FACEAUTH_LABEL_LOGI("type = %{public}s, level = %{public}s", type.c_str(), level.c_str());

    return UseriamAuthKit::CheckAvailability(type, level);
}

napi_value FaceauthMgr::GetAuthenticator(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("enter GetAuthenticator");
    napi_value instance = nullptr;
    napi_value cons = nullptr;
    if (napi_get_reference_value(env, constructorRef_, &cons) != napi_ok) {
        return nullptr;
    }
    if (napi_new_instance(env, cons, 0, nullptr, &instance) != napi_ok) {
        return nullptr;
    }
    return instance;
}

void FaceauthMgr::CreateExecuteAsynch(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo, napi_value callback)
{
    FACEAUTH_LABEL_LOGI("Authentication asyncCallback.");
    napi_value resourceName;
    napi_create_string_latin1(env, "Authentication", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_reference(env, callback, 1, &asyncCallbackInfo->callback);

    napi_create_async_work(
        env, nullptr, resourceName, [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            AsyncFaceAuthCallbackInfo *asyncCallbackInfo = reinterpret_cast<AsyncFaceAuthCallbackInfo *>(data);
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_value result = nullptr;
            napi_create_int32(env, asyncCallbackInfo->ret, &result);
            napi_get_reference_value(env, asyncCallbackInfo->callback, &callback);
            FACEAUTH_LABEL_LOGI("execute authentication asyncCallback.");
            napi_call_function(env, undefined, callback, ARGS_NUMBER_ONE, &result, &callResult);

            if (asyncCallbackInfo->callback != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->callback);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        reinterpret_cast<void *>(asyncCallbackInfo), &asyncCallbackInfo->asyncWork);
}

napi_value FaceauthMgr::CreateExecutePromise(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo)
{
    FACEAUTH_LABEL_LOGI("Authentication promise.");
    napi_deferred deferred;
    napi_value promise;
    napi_create_promise(env, &deferred, &promise);
    asyncCallbackInfo->deferred = deferred;

    napi_value resourceName;
    napi_create_string_latin1(env, "Authentication", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName, [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            AsyncFaceAuthCallbackInfo *asyncCallbackInfo = reinterpret_cast<AsyncFaceAuthCallbackInfo *>(data);
            napi_value result = nullptr;
            napi_create_int32(env, asyncCallbackInfo->ret, &result);
            FACEAUTH_LABEL_LOGI("execute authentication promise.");
            napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        reinterpret_cast<void *>(asyncCallbackInfo), &asyncCallbackInfo->asyncWork);
    return promise;
}

napi_value FaceauthMgr::Execute(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_NUMBER_THREE;
    napi_value argv[ARGS_NUMBER_THREE] = { nullptr };
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    FACEAUTH_LABEL_LOGI("ARGCSIZE is =%{public}d.", argc);
    std::cout << "faceauth execute" << std::endl;
    std::string type;
    std::string level;
    ParseString(env, type, argv[0]);
    ParseString(env, level, argv[1]);
    uint64_t reqId = GetTimestamp() % MAX_REQ_ID_COUNT;
    AsyncFaceAuthCallbackInfo *asyncCallbackInfo = new AsyncFaceAuthCallbackInfo { .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
        .type = type,
        .level = level };
    InsertMap(reqId, asyncCallbackInfo);

    if (argc == ARGS_NUMBER_THREE) {
        CreateExecuteAsynch(env, asyncCallbackInfo, argv[ARGS_NUMBER_TWO]);
        int32_t ret = KitAuthentication(reqId, type, level);
        if (ret == Constant::FAILURE) {
            ret = Constant::JS_FAILURE;
        }
        FACEAUTH_LABEL_LOGI("Authentication callback result = %{public}d", ret);
        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, ret, &result));
        return result;
    } else {
        napi_value promise = CreateExecutePromise(env, asyncCallbackInfo);
        int32_t ret = KitAuthentication(reqId, type, level);
        FACEAUTH_LABEL_LOGI("Authentication callback result = %{public}d", ret);
        return promise;
    }
}

napi_value FaceauthMgr::Cancel(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter Cancel.");
    napi_value result = nullptr;
    int32_t ret = KitCancel(env);
    if (ret == Constant::FAILURE) {
        ret = Constant::JS_FAILURE;
    }
    FACEAUTH_LABEL_LOGI("Cancel result = %{public}d", ret);
    napi_create_int32(env, ret, &result);
    return result;
}

napi_value FaceauthMgr::CheckAvailability(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter CheckAvailability.");
    size_t argc = ARGS_NUMBER_THREE;
    napi_value argv[ARGS_NUMBER_THREE] = { nullptr };
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    FACEAUTH_LABEL_LOGI("ARGCSIZE is =%{public}d.", argc);
    std::string type;
    std::string level;
    ParseString(env, type, argv[0]);
    ParseString(env, level, argv[1]);
    napi_value result = nullptr;
    int32_t ret = KitCheckAvailability(env, type, level);
    FACEAUTH_LABEL_LOGI("CheckAvailability result = %{public}d", ret);
    napi_create_int32(env, ret, &result);
    return result;
}

napi_value FaceauthMgr::GetTips(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter GetTips.");
    napi_value tip = nullptr;
    FACEAUTH_LABEL_LOGI("tip_.tipEvent = %{public}d", tip_.tipEvent);
    FACEAUTH_LABEL_LOGI("tip_.errorCode = %{public}d", tip_.errorCode);
    FACEAUTH_LABEL_LOGI("tip_.tipCode = %{public}d", tip_.tipCode);
    FACEAUTH_LABEL_LOGI("tip_.tipInfo = %{public}s", tip_.tipInfo.c_str());
    SetValueInt32(env, "tipEvent", tip_.tipEvent, tip);
    SetValueInt32(env, "errorCode", tip_.errorCode, tip);
    SetValueInt32(env, "tipCode", tip_.tipCode, tip);
    SetValueString(env, "tipInfo", tip_.tipInfo, tip);
    return tip;
}

napi_value FaceauthMgr::JsOn(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("JsOn in");
    size_t argc = ARGS_NUMBER_TWO;
    napi_value argv[ARGS_NUMBER_TWO] = { nullptr };
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= ARGS_NUMBER_TWO, "Wrong number of arguments, required 2");

    napi_valuetype eventValueType = napi_undefined;
    napi_typeof(env, argv[0], &eventValueType);
    NAPI_ASSERT(env, eventValueType == napi_string, "type mismatch for parameter 1");

    napi_valuetype eventHandleType = napi_undefined;
    napi_typeof(env, argv[1], &eventHandleType);
    NAPI_ASSERT(env, eventHandleType == napi_function, "type mismatch for parameter 2");

    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typeLen);

    NAPI_ASSERT(env, typeLen > 0, "typeLen == 0");
    std::unique_ptr<char[]> type = std::make_unique<char[]>(typeLen + 1);
    napi_get_value_string_utf8(env, argv[0], type.get(), typeLen + 1, &typeLen);

    std::string eventType = type.get();
    napi_unwrap(env, thisArg, reinterpret_cast<void **>(&faceauthNapi_));

    FACEAUTH_LABEL_LOGI("JsOn eventType %{public}s ", eventType.c_str());
    faceauthNapi_->On(eventType, argv[1]);

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value FaceauthMgr::JsOff(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("JsOff in");
    size_t argc = ARGS_NUMBER_TWO;
    napi_value argv[ARGS_NUMBER_TWO] = { nullptr };
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    size_t requireArgc = 1;
    NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments, required 1");
    napi_valuetype eventValueType = napi_undefined;
    napi_typeof(env, argv[0], &eventValueType);
    NAPI_ASSERT(env, eventValueType == napi_string, "type mismatch for parameter 1");
    if (argc > requireArgc) {
        napi_valuetype eventHandleType = napi_undefined;
        napi_typeof(env, argv[1], &eventHandleType);
        NAPI_ASSERT(env, eventHandleType == napi_function, "type mismatch for parameter 2");
    }
    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typeLen);
    NAPI_ASSERT(env, typeLen > 0, "typeLen == 0");
    std::unique_ptr<char[]> type = std::make_unique<char[]>(typeLen + 1);
    napi_get_value_string_utf8(env, argv[0], type.get(), typeLen + 1, &typeLen);
    std::string eventType = type.get();
    if (faceauthNapi_ == nullptr) {
        napi_unwrap(env, thisArg, reinterpret_cast<void **>(&faceauthNapi_));
    }
    FACEAUTH_LABEL_LOGI("JsOff for eventType %{public}s ", eventType.c_str());
    int32_t ret = faceauthNapi_->Off(eventType);
    FACEAUTH_LABEL_LOGI("JsOff for return ret =  %{public}d ", ret);
    napi_ref callbackRef = 0;
    napi_value callback = 0;
    napi_value undefined = 0;
    napi_value callResult = 0;
    napi_get_undefined(env, &undefined);
    napi_value result = nullptr;
    napi_create_int32(env, ret, &result);
    napi_create_reference(env, argv[ARGS_NUMBER_ONE], 1, &callbackRef);
    napi_get_reference_value(env, callbackRef, &callback);
    napi_call_function(env, undefined, callback, ARGS_NUMBER_ONE, &result, &callResult);
    return result;
}

static int32_t InnerkitInit()
{
    FACEAUTH_LABEL_LOGI("InnerkitInit");
    return FaceAuthInnerKit::Init();
}

napi_value FaceauthMgr::JsInit(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter Init.");
    napi_value result = nullptr;
    int32_t ret = InnerkitInit();
    if (ret == Constant::FAILURE) {
        ret = Constant::JS_FAILURE;
    }
    FACEAUTH_LABEL_LOGI("Init result = %{public}d", ret);
    napi_create_int32(env, ret, &result);
    return result;
}

static int32_t InnerkitRelease()
{
    FACEAUTH_LABEL_LOGI("InnerkitRelease");
    return FaceAuthInnerKit::Release();
}

napi_value FaceauthMgr::Release(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter Release.");
    napi_value result = nullptr;
    int32_t ret = InnerkitRelease();
    if (ret == Constant::FAILURE) {
        ret = Constant::JS_FAILURE;
    }
    FACEAUTH_LABEL_LOGI("Release result = %{public}d", ret);
    napi_create_int32(env, ret, &result);
    return result;
}

static napi_value ParseInt32(napi_env env, int32_t &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    FACEAUTH_LABEL_LOGI("param=%{public}d.", valuetype);
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. int32 expected.");
    int32_t value = 0;
    napi_get_value_int32(env, args, &value);
    FACEAUTH_LABEL_LOGI("param=%{public}d.", value);
    param = value;
    // create result code
    napi_value result;
    status = napi_create_int32(env, 1, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static napi_value ParseUint64(napi_env env, uint64_t &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    FACEAUTH_LABEL_LOGI("param=%{public}d.", valuetype);
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. int32 expected.");
    bool isSuccess;
    napi_value result;
    napi_get_value_bigint_uint64(env, args, &param, &isSuccess);
    FACEAUTH_LABEL_LOGI("param=xxxx%04llu.", param);
    // create result code
    status = napi_create_bigint_uint64(env, 1, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static napi_value ParseEnrollParam(napi_env env, EnrollParam &enrollParam, napi_value args)
{
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "param type mismatch!");
    napi_value paramProp = nullptr;
    status = napi_get_named_property(env, args, "enrollParam", &paramProp);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, paramProp, &valueType);
    NAPI_ASSERT(env, valueType == napi_object, "property type mismatch!");
    // get reqId property
    napi_value property = nullptr;
    uint64_t reqId;
    status = napi_get_named_property(env, paramProp, "reqId", &property);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    ParseUint64(env, reqId, property);
    enrollParam.reqId = reqId;
    // get token property
    property = nullptr;
    status = napi_get_named_property(env, paramProp, "token", &property);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, property, &valueType);
    NAPI_ASSERT(env, valueType == napi_string, "property type mismatch!");
    ParseUint8(env, enrollParam.token, property);
    // get XCompentId property
    // get faceId property
    property = nullptr;
    int32_t faceId;
    status = napi_get_named_property(env, paramProp, "faceId", &property);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    ParseInt32(env, faceId, property);
    enrollParam.faceId = faceId;
    // create result code
    napi_value result;
    status = napi_create_int32(env, 1, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static int32_t InnerkitEnroll(EnrollParam &enrollParam)
{
    FACEAUTH_LABEL_LOGI("enrollParam reqId = xxxx%04llu, faceId = %{private}d", enrollParam.reqId, enrollParam.faceId);
    OHOS::sptr<FaceauthMgr::FaceAuthCallback> callback(new FaceauthMgr::FaceAuthCallback());
    return FaceAuthInnerKit::Enroll(enrollParam, callback);
}
void FaceauthMgr::CreateEnrollAsynch(napi_env env, uint64_t reqId, napi_value callback)
{
    FACEAUTH_LABEL_LOGI("CreateEnrollAsynch reqId = xxxx%04llu", reqId);
    AsyncFaceAuthCallbackInfo *asyncCallbackInfo =
        new AsyncFaceAuthCallbackInfo { .env = env, .asyncWork = nullptr, .deferred = nullptr, .reqId = reqId };
    InsertMap(reqId, asyncCallbackInfo);

    FACEAUTH_LABEL_LOGI("Enroll asyncCallback.");
    napi_value resourceName;
    napi_create_string_latin1(env, "Enroll", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_reference(env, callback, 1, &asyncCallbackInfo->callback);

    napi_create_async_work(
        env, nullptr, resourceName, [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            AsyncFaceAuthCallbackInfo *asyncCallbackInfo = reinterpret_cast<AsyncFaceAuthCallbackInfo *>(data);
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_value result;
            napi_create_object(env, &result);
            FaceauthMgr::SetValueInt32(env, "code", asyncCallbackInfo->code, result);
            FaceauthMgr::SetValueInt32(env, "errorCode", asyncCallbackInfo->errorCode, result);
            napi_get_reference_value(env, asyncCallbackInfo->callback, &callback);
            FACEAUTH_LABEL_LOGI("execute enroll asyncCallback.");
            napi_call_function(env, undefined, callback, ARGS_NUMBER_ONE, &result, &callResult);
            if (asyncCallbackInfo->code == Constant::CODE_ACQUIRE) {
                CreateEnrollAsynch(env, asyncCallbackInfo->reqId, callback);
            }
            if (asyncCallbackInfo->callback != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->callback);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        reinterpret_cast<void *>(asyncCallbackInfo), &asyncCallbackInfo->asyncWork);
}

napi_value FaceauthMgr::Enroll(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_NUMBER_TWO;
    napi_value argv[ARGS_NUMBER_TWO] = { nullptr };
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    FACEAUTH_LABEL_LOGI("ARGCSIZE is =%{public}d.", argc);
    EnrollParam enrollParam;
    ParseEnrollParam(env, enrollParam, argv[0]);

    uint64_t reqId = (GetTimestamp() % MAX_REQ_ID_COUNT) + 1;
    while (FaceauthMgr::callbackMap_.count(reqId) > 0) {
        reqId = (GetTimestamp() % MAX_REQ_ID_COUNT) + 1;
    }
    enrollParam.reqId = reqId;

    if (argc != ARGS_NUMBER_TWO) {
        FACEAUTH_LABEL_LOGE("Enroll input param count error!");
    }
    CreateEnrollAsynch(env, reqId, argv[ARGS_NUMBER_ONE]);
    int32_t ret = InnerkitEnroll(enrollParam);
    FACEAUTH_LABEL_LOGI("Enroll callback result = %{public}d", ret);
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, ret, &result));
    return result;
}

static int32_t InnerkitCancelEnroll()
{
    auto it = FaceauthMgr::callbackMap_.begin();
    auto itend = FaceauthMgr::callbackMap_.end();
    if (it != itend) {
        OHOS::sptr<FaceauthMgr::FaceAuthCallback> callback(new FaceauthMgr::FaceAuthCallback());
        uint64_t reqId = it->first;
        FACEAUTH_LABEL_LOGI("KitCancel reqId = xxxx%04llu", reqId);
        return FaceAuthInnerKit::CancelEnrollment(reqId, callback);
    }
    FACEAUTH_LABEL_LOGI("callbackMap is null");
    return Constant::FAILURE;
}

napi_value FaceauthMgr::CancelEnroll(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter CancelEnroll.");
    napi_value result = nullptr;
    int32_t ret = InnerkitCancelEnroll();
    if (ret == Constant::FAILURE) {
        ret = Constant::JS_FAILURE;
    }
    FACEAUTH_LABEL_LOGI("Cancel result = %{public}d", ret);
    napi_create_int32(env, ret, &result);
    return result;
}

static int64_t InnerkitPreEnroll()
{
    FACEAUTH_LABEL_LOGI("InnerkitInit");

    return FaceAuthInnerKit::PreEnroll();
}

napi_value FaceauthMgr::PreEnroll(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter PreEnroll.");
    napi_value result = nullptr;
    int64_t ret = InnerkitPreEnroll();
    FACEAUTH_LABEL_LOGI("PreEnroll result = xxxx%04llu", ret);
    napi_create_int64(env, ret, &result);
    return result;
}

static int32_t InnerkitPostEnroll()
{
    FACEAUTH_LABEL_LOGI("InnerkitPostEnroll");
    return FaceAuthInnerKit::PostEnroll();
}

napi_value FaceauthMgr::PostEnroll(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter PostEnroll.");
    napi_value result = nullptr;
    int32_t ret = InnerkitPostEnroll();
    if (ret == Constant::FAILURE) {
        ret = Constant::JS_FAILURE;
    }
    FACEAUTH_LABEL_LOGI("PostEnroll result = %{public}d", ret);
    napi_create_int32(env, ret, &result);
    return result;
}

static int32_t InnerkitRemove(RemoveParam &removeParam)
{
    OHOS::sptr<FaceauthMgr::FaceAuthCallback> callback(new FaceauthMgr::FaceAuthCallback());
    return FaceAuthInnerKit::Remove(removeParam, callback);
}

void FaceauthMgr::CreateRemoveAsynch(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo, napi_value callback)
{
    FACEAUTH_LABEL_LOGI("Remove asyncCallback.");
    napi_value resourceName;
    napi_create_string_latin1(env, "Remove", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_reference(env, callback, 1, &asyncCallbackInfo->callback);

    napi_create_async_work(
        env, nullptr, resourceName, [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            AsyncFaceAuthCallbackInfo *asyncCallbackInfo = reinterpret_cast<AsyncFaceAuthCallbackInfo *>(data);
            napi_value callback = 0;
            napi_value undefined = 0;
            napi_value callResult = 0;
            napi_get_undefined(env, &undefined);
            napi_value result;
            napi_create_object(env, &result);
            SetValueInt32(env, "code", asyncCallbackInfo->code, result);
            SetValueInt32(env, "errorCode", asyncCallbackInfo->errorCode, result);
            napi_get_reference_value(env, asyncCallbackInfo->callback, &callback);
            napi_call_function(env, undefined, callback, ARGS_NUMBER_ONE, &result, &callResult);

            if (asyncCallbackInfo->callback != nullptr) {
                napi_delete_reference(env, asyncCallbackInfo->callback);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        reinterpret_cast<void *>(asyncCallbackInfo), &asyncCallbackInfo->asyncWork);
}

napi_value FaceauthMgr::CreateRemovePromise(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo)
{
    FACEAUTH_LABEL_LOGI("Remove promise.");
    napi_deferred deferred;
    napi_value promise;
    napi_create_promise(env, &deferred, &promise);
    asyncCallbackInfo->deferred = deferred;

    napi_value resourceName;
    napi_create_string_latin1(env, "Remove", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName, [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            AsyncFaceAuthCallbackInfo *asyncCallbackInfo = reinterpret_cast<AsyncFaceAuthCallbackInfo *>(data);
            napi_value result;
            napi_create_object(env, &result);
            SetValueInt32(env, "code", asyncCallbackInfo->code, result);
            SetValueInt32(env, "errorCode", asyncCallbackInfo->errorCode, result);
            napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        reinterpret_cast<void *>(asyncCallbackInfo), &asyncCallbackInfo->asyncWork);
    return promise;
}

napi_value FaceauthMgr::Remove(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter Remove.");
    size_t argc = ARGS_NUMBER_TWO;
    napi_value argv[ARGS_NUMBER_TWO] = { nullptr };
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    FACEAUTH_LABEL_LOGI("ARGCSIZE is =%{public}d.", argc);
    int32_t faceId;
    ParseInt32(env, faceId, argv[0]);

    uint64_t reqId = (GetTimestamp() % MAX_REQ_ID_COUNT) + 1;
    RemoveParam removeParam;
    removeParam.reqId = reqId;
    removeParam.faceId = faceId;
    AsyncFaceAuthCallbackInfo *asyncCallbackInfo =
        new AsyncFaceAuthCallbackInfo { .env = env, .asyncWork = nullptr, .deferred = nullptr };
    InsertMap(reqId, asyncCallbackInfo);

    if (argc == ARGS_NUMBER_TWO) {
        CreateRemoveAsynch(env, asyncCallbackInfo, argv[ARGS_NUMBER_ONE]);
        int32_t ret = InnerkitRemove(removeParam);
        FACEAUTH_LABEL_LOGI("Remove callback result = %{public}d", ret);
        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, ret, &result));
        return result;
    } else {
        napi_value promise = CreateRemovePromise(env, asyncCallbackInfo);
        int32_t ret = InnerkitRemove(removeParam);
        FACEAUTH_LABEL_LOGI("Remove callback result = %{public}d", ret);
        return promise;
    }
}

static std::list<int32_t> InnerkitGetEnrolledFaceIDs(int32_t userId)
{
    FACEAUTH_LABEL_LOGI("InnerkitPostEnroll");
    return FaceAuthInnerKit::GetEnrolledFaceIDs(userId);
}

napi_value FaceauthMgr::GetEnrolledFaceIds(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter GetEnrolledFaceIds.");

    size_t argc = ARGS_NUMBER_ONE;
    napi_value argv[ARGS_NUMBER_ONE] = { nullptr };
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    int32_t userId;
    ParseInt32(env, userId, argv[0]);

    napi_value result = nullptr;
    auto faceIds = InnerkitGetEnrolledFaceIDs(userId);
    FACEAUTH_LABEL_LOGI("FaceID size = %{public}d", faceIds.size());
    napi_create_array(env, &result);
    size_t ind = 0;
    for (const auto &id : faceIds) {
        FACEAUTH_LABEL_LOGI("FaceID %{public}d = %{public}d", ind, id);
        napi_value faceid;
        napi_create_int32(env, id, &faceid);
        napi_set_element(env, result, ind, faceid);
        ind++;
    }
    return result;
}

static int32_t InnerkitGetAngleDim()
{
    FACEAUTH_LABEL_LOGI("InnerkitGetAngleDim");
    return FaceAuthInnerKit::GetAngleDim();
}

napi_value FaceauthMgr::GetAngleDim(napi_env env, napi_callback_info info)
{
    FACEAUTH_LABEL_LOGI("Enter GetAngleDim.");
    napi_value result = nullptr;
    int32_t ret = InnerkitGetAngleDim();
    FACEAUTH_LABEL_LOGI("GetAngleDim result = %{public}d", ret);
    napi_create_int32(env, ret, &result);
    return result;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    FACEAUTH_LABEL_LOGI("faceauth napi init");
    return OHOS::UserIAM::FaceAuth::FaceauthMgr::Init(env, exports);
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _module = { .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "userIAM.userAuth",
    .nm_priv = ((void *) 0),
    .reserved = { 0 } };
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    FACEAUTH_LABEL_LOGI("RegisterModule() is called!");
    napi_module_register(&_module);
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
