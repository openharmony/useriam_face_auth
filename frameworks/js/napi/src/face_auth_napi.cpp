/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstdint>

#include "js_native_api.h"
#include "js_native_api_types.h"
#include "napi/native_common.h"
#include "node_api.h"

#include "iam_logger.h"

#include "face_auth_defines.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_NAPI

using namespace std;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace {
FaceAuthManager g_faceAuthManager;

napi_value ConvertToNapiValue(napi_env env, int32_t in)
{
    napi_value ret;
    napi_create_int32(env, in, &ret);
    return ret;
}

napi_value FaceAuthManagerConstructor(napi_env env, napi_callback_info info)
{
    IAM_LOGI("start");
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

    NAPI_CALL(env, napi_wrap(env, thisVar, &g_faceAuthManager,
                        [](napi_env env, void *data, void *hint) {}, nullptr, nullptr));

    return thisVar;
}

napi_value FaceAuthManagerSetSurfaceId(napi_env env, napi_callback_info info)
{
    IAM_LOGI("start");
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    FaceAuthManager *manager = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&manager)));
    if (manager == nullptr) {
        IAM_LOGI("unwarp FaceAuthManager error");
        return ConvertToNapiValue(env, FACEAUTH_ERROR);
    }

    return ConvertToNapiValue(env, manager->SetSurfaceId(env, info));
}

napi_value GetFaceAuthManagerConstructor(napi_env env)
{
    IAM_LOGI("start");
    napi_property_descriptor methods[] = {DECLARE_NAPI_FUNCTION("setSurfaceId", FaceAuthManagerSetSurfaceId)};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_define_class(env, "FaceAuth", NAPI_AUTO_LENGTH, FaceAuthManagerConstructor, nullptr,
                       sizeof(methods) / sizeof(napi_property_descriptor), methods, &result));
    return result;
}

napi_value DefineFaceAuthManager(napi_env env, napi_value exports)
{
    IAM_LOGI("start");
    napi_status status = napi_set_named_property(env, exports, "FaceAuthManager", GetFaceAuthManagerConstructor(env));
    NAPI_ASSERT(env, status == napi_ok, "FaceAuthManager napi_set_named_property failed");
    return exports;
}

napi_value GetResultCodeConstructor(napi_env env)
{
    IAM_LOGI("start");
    napi_value resultCode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &resultCode));
    napi_value success = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(FACEAUTH_SUCCESS), &success));
    NAPI_CALL(env, napi_set_named_property(env, resultCode, "SUCCESS", success));
    napi_value fail = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(FACEAUTH_ERROR), &fail));
    NAPI_CALL(env, napi_set_named_property(env, resultCode, "FAIL", fail));
    return resultCode;
}

napi_value DefineEnums(napi_env env, napi_value exports)
{
    IAM_LOGI("start");
    napi_property_descriptor enums[] = {
        DECLARE_NAPI_PROPERTY("ResultCode", GetResultCodeConstructor(env)),
    };
    napi_define_properties(env, exports, sizeof(enums) / sizeof(napi_property_descriptor), enums);
    return exports;
}

napi_value ModuleInit(napi_env env, napi_value exports)
{
    IAM_LOGI("start");
    DefineFaceAuthManager(env, exports);
    DefineEnums(env, exports);
    return exports;
}
} // namespace

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module module = {.nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = ModuleInit,
        .nm_modname = "userIAM.faceAuth",
        .nm_priv = nullptr,
        .reserved = {}};
    napi_module_register(&module);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS