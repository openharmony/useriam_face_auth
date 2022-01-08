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

#include "face_auth_manager.h"
#include <algorithm>
#include <future>
#include <thread>
#include <utility>
#include "faceauth_log_wrapper.h"
#include "face_auth_camera.h"
#include "face_auth_ca.h"
#include "face_auth_defines.h"
#include "face_auth_algo_dispatcher.h"
#include "face_auth_algo_impl.h"
#include "face_auth_thread_pool.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "sa_mgr_client.h"
#include "face_auth_bms_adapter.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
static int32_t errorCount_ = 0;
static long remainingTime_ = 0;
static bool isLockOutMode_ = false;
std::mutex FaceAuthManager::mutex_;
std::shared_ptr<FaceAuthManager> FaceAuthManager::instance_ = nullptr;
std::shared_ptr<FaceAuthManager> FaceAuthManager::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthManager>();
        }
    }
    return instance_;
}

FaceAuthManager::FaceAuthManager() : challenge_(0), iBundleMgr_(nullptr)
{}

FaceAuthManager::~FaceAuthManager()
{}

int32_t FaceAuthManager::Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, challenge: %{private}lld,faceId: %{private}d", param.reqId, param.challenge,
        param.faceId);
    if (CheckEnrollParam(param, callback) != FA_RET_OK) {
        return FA_RET_ERROR;
    }
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    if (param.faceId > 0) {
        if (!CheckFaceIDValidity(param.faceId)) {
            cbParam.errorCode = ERRCODE_NO_FACE_DATA;
            SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
            return FA_RET_ERROR;
        }
    }
    if (FaceAuthAlgoDispatcher::GetInstance()->IsInited()) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::Enroll Need Init.");
        cbParam.errorCode = ERRCODE_ALGORITHM_NOT_INIT;
        SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        return FA_RET_ERROR;
    }
    challenge_ = param.challenge;
    FaceReqType reqType;
    reqType.reqId = param.reqId;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_ENROLL)) {
        FACEAUTH_LABEL_LOGI("EnrollNum is Max.");
        cbParam.errorCode = ERRCODE_BUSY;
        SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        return FA_RET_ERROR;
    }
    FaceInfo faceInfo;
    faceInfo.eventId = GenerateEventId();
    faceInfo.uId = IPCSkeleton::GetCallingUid();
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::HIGH;
    auto authInfo = std::make_unique<CallsEnrollInfo>(param, callback);
    eventHandler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return FA_RET_OK;
}

int32_t FaceAuthManager::CheckEnrollParam(const EnrollParam &param, const sptr<OnFaceAuth> &callback)
{
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    cbParam.errorCode = ERRCODE_INVALID_PARAMETERS;
    if (param.token.size() == 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error. param.token.size: %{public}d", param.token.size());
        SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        return FA_RET_ERROR;
    }
    if (param.faceId < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error. param.faceId: %{private}d", param.faceId);
        SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthManager::Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, flags: %{public}d, challenge: %{private}lld, "
                        "faceId: %{private}d",
        param.reqId, param.flags, param.challenge, param.faceId);
    if (CheckAuthParam(param, callback) != 0) {
        return FA_RET_ERROR;
    }
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    if (FaceAuthAlgoDispatcher::GetInstance()->IsInited()) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::Authenticate Need Init.");
        cbParam.errorCode = ERRCODE_ALGORITHM_NOT_INIT;
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return FA_RET_ERROR;
    }
    if (param.faceId > 0) {
        if (!CheckFaceIDValidity(param.faceId)) {
            FACEAUTH_LABEL_LOGE("FaceAuthManager::Authenticate FaceID not found.");
            cbParam.errorCode = ERRCODE_NO_FACE_DATA;
            SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
            return FA_RET_ERROR;
        }
    }
    if (isLockOutMode_) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::Authenticate Locked.");
        cbParam.errorCode = ERRCODE_IN_LOCKOUT_MODE;
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return FA_RET_ERROR;
    }
    challenge_ = param.challenge;
    if (param.flags == 0) {
        return AddLocalAuth(param, callback);
    } else {
        return AddCoAuth(param, callback);
    }
    return FA_RET_OK;
}

int32_t FaceAuthManager::CheckAuthParam(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    cbParam.errorCode = ERRCODE_INVALID_PARAMETERS;
    if (param.challenge < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error. param.challenge: %{private}lld", param.challenge);
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return FA_RET_ERROR;
    }
    if (param.faceId < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error. param.faceId: %{private}d", param.faceId);
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return FA_RET_ERROR;
    }
    if (param.flags < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error. param.flags: %{public}d", param.flags);
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthManager::Init()
{
    FACEAUTH_LABEL_LOGI("FaceAuthManager::Init");
    std::string bundleName = FaceAuthBmsAdapter::GetInstance()->GetCallingBundleName();
    if (bundleName == "") {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::Init GetCallingBundleName Fail!");
        return FA_RET_ERROR;
    }
    FACEAUTH_LABEL_LOGI("FaceAuthManager::Init end, result ok.");
    return FaceAuthAlgoDispatcher::GetInstance()->Init(bundleName);
}

int32_t FaceAuthManager::Release()
{
    FACEAUTH_LABEL_LOGI("FaceAuthManager::Release");
    std::string bundleName = FaceAuthBmsAdapter::GetInstance()->GetCallingBundleName();
    if (bundleName == "") {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::Release GetCallingBundleName Fail!");
        return FA_RET_ERROR;
    }
    FIRetCode result = FaceAuthAlgoDispatcher::GetInstance()->Release(bundleName);
    if (result != FI_RC_OK) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::Release Fail! result:%{public}d.", result);
        return FA_RET_ERROR;
    }
    FACEAUTH_LABEL_LOGI("FaceAuthManager::Release end, result ok.");
    return FA_RET_OK;
}

void FaceAuthManager::HandleCallEnroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, challenge: %{private}lld, "
                        "faceId: %{private}d",
        param.reqId, param.challenge, param.faceId);
    if (CheckEnrollParam(param, callback) != FA_RET_OK) {
        return;
    }
    sptr<IBufferProducer> producer = param.producer;
    std::promise<int32_t> promiseobj;
    std::future<int32_t> futureobj = promiseobj.get_future();
    FaceReqType reqType;
    reqType.reqId = param.reqId;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    FaceAuthThreadPool::GetInstance()->AddTask([&promiseobj, &producer]() {
        promiseobj.set_value(FaceAuthCamera::GetInstance()->OpenCamera(producer));
    });
    Prepare(HW_EXEC_TYPE_ENROOL);
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    auto token_ptr = std::make_unique<u_int8_t[]>(sizeof(param.token));
    if (memcpy_s(token_ptr.get(), TOKEN_NUM, &param.token[0], sizeof(param.token)) != EOK) {
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        cbParam.errorCode = ERRCODE_FAIL;
        SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        return;
    }
    for (std::vector<uint8_t>::const_iterator iter = param.token.begin(); iter != param.token.end(); ++iter) {
        cbParam.authtoken.push_back(*iter);
    }
    FACEAUTH_LABEL_LOGI("cbtoken:%{public}d, ptoken:%{public}d", cbParam.authtoken.size(), sizeof(param.token));
    VerifyAuthToken(std::move(token_ptr), sizeof(param.token), 0);
    std::chrono::microseconds span(OPEN_CAMERA_TIME_OUT);
    while (futureobj.wait_for(span) == std::future_status::timeout) {
        FACEAUTH_LABEL_LOGI("Open Camera TimeOut");
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        cbParam.errorCode = ERRCODE_TIMEOUT;
        SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        return;
    }
    if (futureobj.get() == FA_RET_ERROR) {
        FACEAUTH_LABEL_LOGE("Open Camera Fail");
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        cbParam.errorCode = ERRCODE_CAMERA_FAIL;
        SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        return;
    }
    ExecuteEnrollEvent(param.reqId, param.faceId, cbParam.authtoken, callback);
    return;
}

void FaceAuthManager::HandleCallAuthenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, flags: %{public}d, challenge: %{private}lld, "
                        "faceId: %{private}d",
        param.reqId, param.flags, param.challenge, param.faceId);

    if (CheckAuthParam(param, callback) != FA_RET_OK) {
        return;
    }
    // Asynchronously call STD:: async to configure and turn on the camera
    sptr<IBufferProducer> producer = param.producer;
    std::promise<int32_t> promiseobj;
    std::future<int32_t> futureobj = promiseobj.get_future();
    FaceReqType reqType = CreateAuthReqInfo(param.reqId, param.flags);
    FACEAUTH_LABEL_LOGI("FaceAuthCurTaskNum is %{public}d ", FaceAuthThreadPool::GetInstance()->GetCurTaskNum());
    FaceAuthThreadPool::GetInstance()->AddTask([&promiseobj, &producer]() {
        promiseobj.set_value(FaceAuthCamera::GetInstance()->OpenCamera(producer));
    });
    Prepare(HW_EXEC_TYPE_UNLOCK);
    SetChallenge(param.challenge);
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    std::chrono::microseconds span(OPEN_CAMERA_TIME_OUT);
    while (futureobj.wait_for(span) == std::future_status::timeout) {
        FACEAUTH_LABEL_LOGI("Open Camera TimeOut");
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        cbParam.errorCode = ERRCODE_TIMEOUT;
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return;
    }
    if (futureobj.get() == FA_RET_ERROR) {
        FACEAUTH_LABEL_LOGE("Open Camera Fail");
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        cbParam.errorCode = ERRCODE_CAMERA_FAIL;
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return;
    }
    ExecuteAuthEvent(param.reqId, param.flags, callback);
    return;
}

void FaceAuthManager::HandleCallRemove(const RemoveParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, faceId: %{private}d", param.reqId, param.faceId);
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    FaceReqType reqType;
    reqType.reqId = param.reqId;
    reqType.operateType = FACE_OPERATE_TYPE_DEL;
    // Because no ERRCODE_INVALID_PARAMETERS in Remove,ERRCODE_FAIL used.
    cbParam.errorCode = ERRCODE_FAIL;
    if (param.faceId < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.param.faceId is %{private}d", param.faceId);
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        SendCallback(TYPE_CALLBACK_REMOVE, cbParam, callback);
        return;
    }
    ExecuteRemoveEvent(param, callback);
    return;
}

void FaceAuthManager::ExecuteEnrollEvent(uint64_t reqId, int32_t faceId, std::vector<uint8_t> token,
    const sptr<OnFaceAuth> &callback)
{
    int32_t authErrorCode;
    FICode code;
    CallBackParam cbParam;
    cbParam.reqId = reqId;
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        cbParam.authtoken.push_back(*iter);
    }
    FaceReqType reqType;
    reqType.reqId = reqId;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    FACEAUTH_LABEL_LOGI("cbParam.authtoken is %{public}d", cbParam.authtoken.size());
    do {
        std::promise<int32_t> promiseobj;
        std::future<int32_t> futureobj = promiseobj.get_future();
        FACEAUTH_LABEL_LOGI("FaceAuthCurTaskNum is %{public}d ", FaceAuthThreadPool::GetInstance()->GetCurTaskNum());
        FaceAuthThreadPool::GetInstance()->AddTask([&promiseobj, &authErrorCode, &code, &reqId]() {
            promiseobj.set_value(FaceAuthAlgoDispatcher::GetInstance()->GetEnrollResult(authErrorCode, code, reqId));
        });
        std::chrono::microseconds span(GET_RESULT_TIME_OUT);
        while (futureobj.wait_for(span) == std::future_status::timeout) {
            FACEAUTH_LABEL_LOGI("GetEnrollResult TimeOut");
            cbParam.errorCode = ERRCODE_TIMEOUT;
            HandleExceptionCallback(TYPE_CALLBACK_ENROLL, cbParam, FACE_ERROR_TIMEOUT, callback);
            return;
        }
        if (futureobj.get() != FI_RC_OK) {
            FACEAUTH_LABEL_LOGI("GetEnrollResult Fail");
            cbParam.errorCode = ERRCODE_FAIL;
            HandleExceptionCallback(TYPE_CALLBACK_ENROLL, cbParam, FACE_ERROR_FAIL, callback);
        }
        if (code != CODE_CALLBACK_RESULT && code != CODE_CALLBACK_FACEID) {
            cbParam.code = CODE_CALLBACK_ACQUIRE;
            cbParam.errorCode = authErrorCode;
            SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
        }
    } while (code != CODE_CALLBACK_RESULT);
    FaceAuthAlgoDispatcher::GetInstance()->Reset(SwitchFaceErrorCode(authErrorCode));
    FaceAuthCamera::GetInstance()->CloseCamera();
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
    cbParam.code = CODE_CALLBACK_RESULT;
    cbParam.errorCode = authErrorCode;
    SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
    return;
}

void FaceAuthManager::ExecuteRemoveEvent(const RemoveParam &param, const sptr<OnFaceAuth> &callback)
{
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    FaceReqType reqType;
    reqType.reqId = param.reqId;
    reqType.operateType = FACE_OPERATE_TYPE_DEL;
    int32_t result = FaceAuthAlgoDispatcher::GetInstance()->DeleteFace(param.faceId);
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
    if (FA_RET_OK == result) {
        FACEAUTH_LABEL_LOGI("Remove success");
        cbParam.errorCode = ERRCODE_SUCCESS;
        SendCallback(TYPE_CALLBACK_REMOVE, cbParam, callback);
    } else {
        FACEAUTH_LABEL_LOGI("Remove fail");
        cbParam.errorCode = ERRCODE_FAIL;
        SendCallback(TYPE_CALLBACK_REMOVE, cbParam, callback);
    }
    return;
}

FaceReqType FaceAuthManager::CreateAuthReqInfo(uint64_t reqId, uint32_t flags)
{
    FaceReqType reqType;
    reqType.reqId = reqId;
    if (flags == 0) {
        reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    } else {
        reqType.operateType = FACE_OPERATE_TYPE_CO_AUTH;
    }
    return reqType;
}

void FaceAuthManager::ExecuteAuthEvent(const uint64_t reqId, const int32_t flags, const sptr<OnFaceAuth> &callback)
{
    int32_t authErrorCode;
    FICode code;
    CallBackParam cbParam;
    cbParam.reqId = reqId;
    cbParam.flags = flags;
    FaceReqType reqType = CreateAuthReqInfo(reqId, flags);
    do {
        std::promise<int32_t> promiseobj;
        std::future<int32_t> futureobj = promiseobj.get_future();
        FACEAUTH_LABEL_LOGI("FaceAuthCurTaskNum is %{public}d ", FaceAuthThreadPool::GetInstance()->GetCurTaskNum());
        FaceAuthThreadPool::GetInstance()->AddTask([&promiseobj, &authErrorCode, &code, &reqId]() {
            promiseobj.set_value(FaceAuthAlgoDispatcher::GetInstance()->GetAuthResult(authErrorCode, code, reqId));
        });
        std::chrono::microseconds span(GET_RESULT_TIME_OUT);
        while (futureobj.wait_for(span) == std::future_status::timeout) {
            FACEAUTH_LABEL_LOGI("GetAuthResult TimeOut");
            cbParam.errorCode = ERRCODE_TIMEOUT;
            HandleExceptionCallback(TYPE_CALLBACK_AUTH, cbParam, FACE_ERROR_TIMEOUT, callback);
            return;
        }
        if (futureobj.get() != FI_RC_OK) {
            FACEAUTH_LABEL_LOGI("GetAuthResult Fail");
            cbParam.errorCode = ERRCODE_FAIL;
            HandleExceptionCallback(TYPE_CALLBACK_AUTH, cbParam, FACE_ERROR_FAIL, callback);
        }
        if (code != CODE_CALLBACK_RESULT && code != CODE_CALLBACK_FACEID) {
            cbParam.code = CODE_CALLBACK_ACQUIRE;
            cbParam.errorCode = authErrorCode;
            SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        }
    } while (code != CODE_CALLBACK_RESULT);
    if (authErrorCode == ERRCODE_COMPARE_FAIL) {
        errorCount_++;
        FACEAUTH_LABEL_LOGI("Compare Fail errorCount_ +1,errorCount_ is %{public}d ", errorCount_);
    }
    if (CheckLockOutMode(reqId, callback) == true) {
        return;
    }
    if (authErrorCode == ERRCODE_SUCCESS) {
        FACEAUTH_LABEL_LOGI("Authenticate Success,errorCount_ = 0");
        errorCount_ = 0;
    }
    FaceAuthAlgoDispatcher::GetInstance()->Reset(SwitchFaceErrorCode(authErrorCode));
    FaceAuthCamera::GetInstance()->CloseCamera();
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
    cbParam.code = CODE_CALLBACK_RESULT;
    cbParam.errorCode = authErrorCode;
    SendAuthCallbackByChallenge(cbParam, callback);
    return;
}

void FaceAuthManager::HandleExceptionCallback(const FIResultType type, CallBackParam cbParam, const int32_t resetValue,
    const sptr<OnFaceAuth> &callback)
{
    FaceReqType reqType;
    reqType.reqId = cbParam.reqId;
    if (type == TYPE_CALLBACK_ENROLL) {
        reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    } else if (type == TYPE_CALLBACK_REMOVE) {
        reqType.operateType = FACE_OPERATE_TYPE_DEL;
    } else {
        if (cbParam.flags == 0) {
            reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
        } else {
            reqType.operateType = FACE_OPERATE_TYPE_CO_AUTH;
        }
    }
    cbParam.code = CODE_CALLBACK_RESULT;
    FaceAuthAlgoDispatcher::GetInstance()->Reset(resetValue);
    FaceAuthCamera::GetInstance()->CloseCamera();
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
    SendCallback(type, cbParam, callback);
    return;
}

bool FaceAuthManager::CheckLockOutMode(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    if (errorCount_ >= AUTH_FAIL_MAX_TIMES) {
        isLockOutMode_ = true;
        FACEAUTH_LABEL_LOGI("FaceAuthCurTaskNum is %{public}d ", FaceAuthThreadPool::GetInstance()->GetCurTaskNum());
        FaceAuthThreadPool::GetInstance()->AddTask([this]() {
            StartTimer();
        });
        CallBackParam cbParam;
        cbParam.reqId = reqId;
        cbParam.code = CODE_CALLBACK_RESULT;
        cbParam.errorCode = ERRCODE_IN_LOCKOUT_MODE;
        HandleExceptionCallback(TYPE_CALLBACK_AUTH, cbParam, FACE_ERROR_COMPARE_FAIL, callback);
        return true;
    }
    return false;
}

void FaceAuthManager::SendAuthCallbackByChallenge(CallBackParam cbParam, const sptr<OnFaceAuth> &callback)
{
    if (challenge_ > 0) {
        uint32_t len;
        std::unique_ptr<u_int8_t[]> token_ptr = std::make_unique<u_int8_t[]>(TOKEN_NUM);
        FaceAuthAlgoDispatcher::GetInstance()->GetAuthToken(token_ptr, len);
        uint8_t token[TOKEN_NUM];
        if (memcpy_s(token, TOKEN_NUM, token_ptr.get(), len) != EOK) {
            FACEAUTH_LABEL_LOGE("GetAuthToken memcpy_s fail");
            cbParam.errorCode = ERRCODE_FAIL;
            SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
            return;
        }
        for (size_t i = 0; i < TOKEN_NUM; i++) {
            cbParam.authtoken.push_back(token[i]);
        }
    }
    SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
    return;
}

void FaceAuthManager::StartTimer()
{
    for (remainingTime_ = AUTH_FAIL_WAIT_TIME; remainingTime_ > 0; remainingTime_--) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    isLockOutMode_ = false;
    errorCount_ = 0;
    return;
}

void FaceAuthManager::ResetTimeout()
{
    remainingTime_ = 0;
    isLockOutMode_ = false;
    FACEAUTH_LABEL_LOGI("FaceAuthManager::ResetTimeout in");
    errorCount_ = 0;
    return;
}

int32_t FaceAuthManager::GetRemainingNum()
{
    FACEAUTH_LABEL_LOGI("errorCount_ is %{public}d", errorCount_);
    return AUTH_FAIL_MAX_TIMES - errorCount_;
}

long FaceAuthManager::GetRemainingTime()
{
    FACEAUTH_LABEL_LOGI("remainingTime_ is %{public}ld", remainingTime_);
    return remainingTime_;
}

int32_t FaceAuthManager::CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    int32_t result = FA_RET_OK;
    FaceReqType reqType;
    reqType.reqId = reqId;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    int32_t uId = IPCSkeleton::GetCallingUid();
    bool isSuccess = FaceAuthReq::GetInstance()->SetCancelFlag(reqType, uId);
    if (!isSuccess) {
        reqType.operateType = FACE_OPERATE_TYPE_CO_AUTH;
        isSuccess = FaceAuthReq::GetInstance()->SetCancelFlag(reqType, uId);
        if (!isSuccess) {
            FACEAUTH_LABEL_LOGE("CancelAuth failed,reqId: xxxx%04llu, ", reqId);
            result = FA_RET_ERROR;
        }
    }
    if (isSuccess) {
        FaceAuthAlgoDispatcher::GetInstance()->Cancel(reqId);
    }
    CallBackParam cbParam;
    cbParam.reqId = reqId;
    cbParam.code = CODE_CALLBACK_CANCEL;
    SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
    return result;
}

int32_t FaceAuthManager::CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    int32_t result = FA_RET_OK;
    FaceReqType reqType;
    reqType.reqId = reqId;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    int32_t uId = IPCSkeleton::GetCallingUid();
    bool isSuccess = FaceAuthReq::GetInstance()->SetCancelFlag(reqType, uId);
    if (!isSuccess) {
        FACEAUTH_LABEL_LOGE("CancelEnrollment failed,reqId: xxxx%04llu, ", reqId);
        result = FA_RET_ERROR;
    } else {
        FaceAuthAlgoDispatcher::GetInstance()->Cancel(reqId);
    }
    CallBackParam cbParam;
    cbParam.reqId = reqId;
    cbParam.code = CODE_CALLBACK_CANCEL;
    SendCallback(TYPE_CALLBACK_ENROLL, cbParam, callback);
    return result;
}

int32_t FaceAuthManager::Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu,faceId: %{private}d", param.reqId, param.faceId);
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    // Because no ERRCODE_INVALID_PARAMETERS in Remove,ERRCODE_FAIL used.
    if (param.faceId < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error. param.faceId: %{private}d", param.faceId);
        cbParam.errorCode = ERRCODE_FAIL;
        SendCallback(TYPE_CALLBACK_REMOVE, cbParam, callback);
        return FA_RET_ERROR;
    }
    if (FaceAuthAlgoDispatcher::GetInstance()->IsInited()) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::Remove Need Init.");
        cbParam.errorCode = ERRCODE_ALGORITHM_NOT_INIT;
        SendCallback(TYPE_CALLBACK_REMOVE, cbParam, callback);
        return FA_RET_ERROR;
    }
    FaceReqType reqType;
    reqType.reqId = param.reqId;
    reqType.operateType = FACE_OPERATE_TYPE_DEL;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_DEL)) {
        FACEAUTH_LABEL_LOGI("Remove is Max.");
        cbParam.errorCode = ERRCODE_BUSY;
        SendCallback(TYPE_CALLBACK_REMOVE, cbParam, callback);
        return FA_RET_ERROR;
    }
    FaceInfo faceInfo;
    faceInfo.eventId = GenerateEventId();
    faceInfo.uId = IPCSkeleton::GetCallingUid();
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::IMMEDIATE;
    auto authInfo = std::make_unique<CallsRemoveInfo>(param, callback);
    eventHandler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return FA_RET_OK;
}

void FaceAuthManager::SendCallback(const FIResultType type, const CallBackParam cbParam,
    const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("type: %{public}d, reqId: xxxx%04llu, code: %{public}d, errorCode: %{public}d", type,
        cbParam.reqId, cbParam.code, cbParam.errorCode);
    Bundle bundleInfo;
    for (std::vector<uint8_t>::const_iterator iter = cbParam.authtoken.begin(); iter != cbParam.authtoken.end();
         ++iter) {
        bundleInfo.authtoken.push_back(*iter);
    }
    FACEAUTH_LABEL_LOGI("bundleInfo.authtoken is %{public}d", bundleInfo.authtoken.size());
    callback->OnCallbackEvent(cbParam.reqId, type, cbParam.code, cbParam.errorCode, bundleInfo);
    return;
}

int32_t FaceAuthManager::AddLocalAuth(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_LOCAL_AUTH)) {
        FACEAUTH_LABEL_LOGI("Authenticate is Max.");
        cbParam.errorCode = ERRCODE_BUSY;
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return FA_RET_ERROR;
    }
    FaceReqType reqType;
    reqType.reqId = param.reqId;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    FaceInfo faceInfo;
    faceInfo.eventId = GenerateEventId();
    faceInfo.uId = IPCSkeleton::GetCallingUid();
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::LOW;
    auto authInfo = std::make_unique<CallsAuthInfo>(param, callback);
    eventHandler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return FA_RET_OK;
}

int32_t FaceAuthManager::AddCoAuth(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    CallBackParam cbParam;
    cbParam.reqId = param.reqId;
    cbParam.code = CODE_CALLBACK_RESULT;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_CO_AUTH)) {
        FACEAUTH_LABEL_LOGI("CoAuthenticate is Max.");
        cbParam.errorCode = ERRCODE_BUSY;
        SendCallback(TYPE_CALLBACK_AUTH, cbParam, callback);
        return FA_RET_ERROR;
    }
    FaceReqType reqType;
    reqType.reqId = param.reqId;
    reqType.operateType = FACE_OPERATE_TYPE_CO_AUTH;
    FaceInfo faceInfo;
    faceInfo.eventId = GenerateEventId();
    faceInfo.uId = IPCSkeleton::GetCallingUid();
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FACEAUTH_LABEL_LOGI("AddCoAuth start");
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::LOW;
    auto authInfo = std::make_unique<CallsAuthInfo>(param, callback);
    eventHandler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    FACEAUTH_LABEL_LOGI("AddCoAuth end");
    return FA_RET_OK;
}

void FaceAuthManager::Prepare(HWExeType type)
{
    std::unique_ptr<PrepareParam> preparam_ptr = std::make_unique<PrepareParam>();
    FaceAuthAlgoDispatcher::GetInstance()->Prepare(type, std::move(preparam_ptr));
    return;
}

void FaceAuthManager::SetChallenge(int64_t challenge)
{
    if (challenge < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.challenge is %{private}lld", challenge);
        return;
    }
    if (challenge == 0) {
        FACEAUTH_LABEL_LOGI("challenge is 0");
        return;
    }
    FaceAuthAlgoDispatcher::GetInstance()->SetChallenge(challenge);
    return;
}

int32_t FaceAuthManager::SendCameraImage(std::unique_ptr<CameraImageStream> img)
{
    if (img == nullptr) {
        FACEAUTH_LABEL_LOGE("Parameter check error.img == nullptr");
        return FA_RET_ERROR;
    }
    FaceAuthAlgoDispatcher::GetInstance()->TransformImage(std::move(img), 0);
    return FA_RET_OK;
}

std::list<int32_t> FaceAuthManager::GetEnrolledFaceIDs(const int32_t userId)
{
    std::list<int32_t> faceInfolist;
    if (userId != 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.userId is %{public}d", userId);
        return faceInfolist;
    }
    FaceAuthAlgoDispatcher::GetInstance()->SetActiveGroup(userId, faceInfolist);
    return faceInfolist;
}

bool FaceAuthManager::CheckFaceIDValidity(int32_t faceId)
{
    std::list<int32_t> enrolled_faceids = GetEnrolledFaceIDs(USER_ID_DEFAULT);
    FACEAUTH_LABEL_LOGI("faceId: %{public}d,enrolled_faceids size: %{public}d,enrolled_faceids[0]:%{public}d", faceId,
        enrolled_faceids.front(), enrolled_faceids.size());
    if (std::find(enrolled_faceids.begin(), enrolled_faceids.end(), faceId) != enrolled_faceids.end()) {
        return true;
    }
    return false;
}

uint32_t FaceAuthManager::SwitchFaceErrorCode(int32_t authErrorCode)
{
    uint32_t faceErrorCode;
    switch (authErrorCode) {
        case ERRCODE_SUCCESS:
            faceErrorCode = FACE_SUCCESS;
            break;
        case ERRCODE_CANCEL:
            faceErrorCode = FACE_ERROR_CANCEL;
            break;
        case ERRCODE_TIMEOUT:
            faceErrorCode = FACE_ERROR_TIMEOUT;
            break;
        case ERRCODE_COMPARE_FAIL:
            faceErrorCode = FACE_ERROR_COMPARE_FAIL;
            break;
        default:
            faceErrorCode = FACE_SUCCESS;
            break;
    }
    return faceErrorCode;
}

int32_t FaceAuthManager::PreEnroll(int64_t &challenge, int32_t type)
{
    FIRetCode result = FaceAuthAlgoDispatcher::GetInstance()->PreEnroll(challenge, type);
    FACEAUTH_LABEL_LOGI("FaceAuthManager PreEnroll result:%{public}d", result);
    challenge_ = challenge;
    if (result != FI_RC_OK) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager PreEnroll fail ");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthManager::GetAngleDim()
{
    FACEAUTH_LABEL_LOGI("FaceAuthManager GetAngleDim");
    int32_t angleNum = 0;
    FaceAuthAlgoDispatcher::GetInstance()->GetEnrollPoseCount(angleNum);
    return angleNum;
}

int32_t FaceAuthManager::VerifyAuthToken(std::unique_ptr<uint8_t[]> authToken, uint32_t length, int type)
{
    FACEAUTH_LABEL_LOGI("FaceAuthManager VerifyAuthToken");
    FIRetCode result = FaceAuthAlgoDispatcher::GetInstance()->VerifyAuthToken(std::move(authToken), length, type);
    if (result != FI_RC_OK) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager VerifyAuthToken fail ");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthManager::PostEnroll(int32_t type)
{
    FIRetCode result = FaceAuthAlgoDispatcher::GetInstance()->PostEnroll(type);
    if (result != 0) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager postEnroll fail ");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

uint32_t FaceAuthManager::GenerateEventId()
{
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &t);
    int64_t elapsedTime { ((t.tv_sec) * SEC_TO_NANOSEC + t.tv_nsec) };
    size_t elapsedHash = std::hash<std::string>()(std::to_string(elapsedTime));
    uint32_t eventId = static_cast<uint32_t>(elapsedHash);
    FACEAUTH_LABEL_LOGI("GenerateEventId generate eventId %{public}u", eventId);
    return eventId;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS