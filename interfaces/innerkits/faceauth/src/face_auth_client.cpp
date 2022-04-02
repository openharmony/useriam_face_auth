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

#include "face_auth_client.h"
#include "face_auth_log_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::shared_ptr<FaceAuthClient> FaceAuthClient::instance_ = nullptr;
std::mutex FaceAuthClient::mutex_;
FaceAuthClient::FaceAuthClient() = default;
FaceAuthClient::~FaceAuthClient() = default;

int32_t FaceAuthClient::SetBufferProducer(sptr<IBufferProducer> &producer)
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
    sptr<IFaceAuth> proxy = GetFaceAuthProxy();
    if (proxy == nullptr) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "get faceAuthProxy fail");
        return FA_RET_ERROR;
    }
    return proxy->SetBufferProducer(producer);
}

void FaceAuthClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
    std::lock_guard<std::mutex> lock(mutex_);
    if ((faceAuthProxy_ != nullptr) && (faceAuthProxy_->AsObject() != nullptr)) {
        faceAuthProxy_->AsObject()->RemoveDeathRecipient(recipient_);
        recipient_ = nullptr;
    }
    faceAuthProxy_ = nullptr;
    return;
}

sptr<IFaceAuth> FaceAuthClient::GetFaceAuthProxy()
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (faceAuthProxy_ != nullptr) {
        return faceAuthProxy_;
    }
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to get systemAbilityManager.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH);
    if (remoteObject == nullptr) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to get remoteObject.");
        return nullptr;
    }

    faceAuthProxy_ = iface_cast<IFaceAuth>(remoteObject);
    if ((faceAuthProxy_ == nullptr) || (faceAuthProxy_->AsObject() == nullptr)) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to get faceAuthProxy_");
        return nullptr;
    }

    faceAuthProxy_->AsObject()->AddDeathRecipient(this);
    return faceAuthProxy_;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
