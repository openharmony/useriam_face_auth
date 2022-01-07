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

#include "face_auth_death_recipient.h"
#include "faceauth_log_wrapper.h"
#include "face_auth_client.h"
#include "singleton.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthDeathRecipient::FaceAuthDeathRecipient(const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("init FaceAuthDeathRecipient");
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGI("init FaceAuthDeathRecipient empty input");
    }
    if (callback_ == nullptr) {
        callback_ = callback;
    }
}

void FaceAuthDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    FACEAUTH_LABEL_LOGI("enter");
    if (callback_ == nullptr) {
        FACEAUTH_LABEL_LOGI("empty callback pointer");
        return;
    }
    callback_->OnDeathRecipient();
    DelayedSingleton<FaceAuthClient>::GetInstance()->ResetFaceAuthProxy();
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
