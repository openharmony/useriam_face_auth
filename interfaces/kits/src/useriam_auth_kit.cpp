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

#include "useriam_auth_kit.h"
#include "faceauth_log_wrapper.h"
#include "singleton.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
int32_t UseriamAuthKit::Execute(
    const AuthParam &param, const std::string &type, const std::string &level, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI(" Execute  enter, reqId = xxxx%04llu, type = %{public}s, level = %{public}s",
        param.reqId,
        type.c_str(),
        level.c_str());
    if (type != Constant::FACE_ONLY) {
        FACEAUTH_LABEL_LOGI(" Wrong type: %{public}s", type.c_str());
        Bundle bundleInfo;
        if (type != Constant::ALL) {
            callback->OnCallbackEvent(
                param.reqId, TYPE_CALLBACK_AUTH, CODE_CALLBACK_RESULT, ERRCODE_INVALID_PARAMETERS, bundleInfo);
            return Constant::FAILURE;
        }
        callback->OnCallbackEvent(param.reqId, TYPE_CALLBACK_AUTH, CODE_CALLBACK_RESULT, FACE_NONE, bundleInfo);
        return Constant::FAILURE;
    }
    FaceAuthInnerKit::Init();
    return FaceAuthInnerKit::Authenticate(param, callback);
}

int32_t UseriamAuthKit::Cancel(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI(" Cancel  enter, reqId = xxxx%04llu", reqId);
    return FaceAuthInnerKit::CancelAuth(reqId, callback);
}

int32_t UseriamAuthKit::CheckAvailability(const std::string &type, const std::string &level)
{
    FACEAUTH_LABEL_LOGI(
        " CheckAvailability  enter, type = %{public}s, level = %{public}s", type.c_str(), level.c_str());
    if (type != Constant::FACE_ONLY && type != Constant::ALL) {
        FACEAUTH_LABEL_LOGI(" Wrong type: %{public}s", type.c_str());
        return AUTH_TYPE_NOT_SUPPORT;
    }
    if (level != Constant::LEVEL_1 && level != Constant::LEVEL_2 && level != Constant::LEVEL_3 &&
        level != Constant::LEVEL_4) {
        return SECURE_LEVEL_NOT_SUPPORT;
    }
    return SUPPORTED;
}
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS