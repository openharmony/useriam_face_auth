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

#include "face_auth_log_wrapper.h"
#include "coauth_info_define.h"
#include "face_auth_common_event_subscriber.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const std::string REGISTER_NOTIFICATION = "EXECUTOR_REGISTER_NOTIFICATION";

FaceAuthCommonEventSubscriber::FaceAuthCommonEventSubscriber(
    const CommonEventSubscribeInfo &subscribeInfo, FaceAuthService* callback)
    : CommonEventSubscriber(subscribeInfo), callback_(callback)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthCommonEventSubscriber enter");
}

void FaceAuthCommonEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "OnReceiveEvent enter");
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = want.GetAction();
    FACEAUTH_HILOGD(MODULE_SERVICE, "Received common event:%{public}s", action.c_str());
    if (action == REGISTER_NOTIFICATION) {
        callback_->Start();
        return;
    }
}
} // namespace FaceAuth
} // namespace UserIAM
}  // namespace OHOS
