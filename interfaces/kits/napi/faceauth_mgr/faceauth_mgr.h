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

#ifndef FACEAUTH_MGR_H_
#define FACEAUTH_MGR_H_
#include <vector>
#include <pthread.h>
#include <cstdio>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <ctime>
#include <map>
#include "face_auth_defines.h"
#include "useriam_auth_kit.h"
#include "face_auth_innerkit.h"
#include "faceauth_native_event.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "on_faceauth_stub.h"
#include "napi/native_api.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
struct AsyncFaceAuthCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback = 0;
    std::string type;
    std::string level;
    uint64_t reqId;
    int32_t faceId;
    int32_t ret;
    int32_t code;
    int32_t errorCode;
};

struct Tips {
    int32_t tipEvent;
    int32_t errorCode;
    int32_t tipCode;
    std::string tipInfo;
    Tips() noexcept;
};

class FaceauthMgr : public FaceAuthNativeEvent {
public:
    class FaceAuthCallback;
    explicit FaceauthMgr(napi_env env, napi_value thisVar);
    ~FaceauthMgr() override;
    static napi_value Init(napi_env env, napi_value exports);
    static void InsertMap(int64_t reqId, AsyncFaceAuthCallbackInfo *asyncCallbackInfo);
    static void SetValueInt32(const napi_env &env, const std::string &fieldStr, const int intValue, napi_value &result);
    static void SetValueString(const napi_env &env, const std::string &fieldStr, const std::string &strValue,
        napi_value &result);
    static void CreateEnrollAsynch(napi_env env, uint64_t reqId, napi_value callback);
    static void CreateExecuteAsynch(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo, napi_value callback);
    static void CreateRemoveAsynch(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo, napi_value callback);
    static napi_value CreateExecutePromise(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo);
    static napi_value CreateRemovePromise(napi_env env, AsyncFaceAuthCallbackInfo *asyncCallbackInfo);
    static Tips tip_;
    static std::mutex mutex_;
    static std::map<uint64_t, AsyncFaceAuthCallbackInfo *> callbackMap_;

private:
    static napi_value GetAuthenticator(napi_env env, napi_callback_info info);
    static napi_value Execute(napi_env env, napi_callback_info info);
    static napi_value Cancel(napi_env env, napi_callback_info info);
    static napi_value CheckAvailability(napi_env env, napi_callback_info info);
    static napi_value GetTips(napi_env env, napi_callback_info info);
    static napi_value JsOn(napi_env env, napi_callback_info info);
    static napi_value JsOff(napi_env env, napi_callback_info info);
    static napi_value JsInit(napi_env env, napi_callback_info info);
    static napi_value Release(napi_env env, napi_callback_info info);
    static napi_value Enroll(napi_env env, napi_callback_info info);
    static napi_value CancelEnroll(napi_env env, napi_callback_info info);
    static napi_value PreEnroll(napi_env env, napi_callback_info info);
    static napi_value PostEnroll(napi_env env, napi_callback_info info);
    static napi_value Remove(napi_env env, napi_callback_info info);
    static napi_value GetEnrolledFaceIds(napi_env env, napi_callback_info info);
    static napi_value GetAngleDim(napi_env env, napi_callback_info info);
    static napi_value JsConstructor(napi_env env, napi_callback_info info);
    napi_ref wrapper_;
    static napi_ref constructorRef_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif /* FACEAUTH_MGR_H_ */
