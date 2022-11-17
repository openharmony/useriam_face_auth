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

#include <cstdint>
#include <mutex>

#include "if_system_ability_manager.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "refbase.h"
#include "system_ability_definition.h"

#include "iam_logger.h"

#include "face_auth_defines.h"
#include "iface_auth.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SDK

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
int32_t FaceAuthClient::SetBufferProducer(sptr<IBufferProducer> &producer)
{
    IAM_LOGI("start");
    sptr<IFaceAuth> proxy = GetFaceAuthProxy();
    if (proxy == nullptr) {
        IAM_LOGE("get faceAuthProxy fail");
        return FACE_AUTH_ERROR;
    }
    return proxy->SetBufferProducer(producer);
}

void FaceAuthClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    IAM_LOGI("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if ((faceAuthProxy_ != nullptr) && (faceAuthProxy_->AsObject() != nullptr)) {
        faceAuthProxy_->AsObject()->RemoveDeathRecipient(this);
    }
    faceAuthProxy_ = nullptr;
    return;
}

sptr<IFaceAuth> FaceAuthClient::GetFaceAuthProxy()
{
    IAM_LOGI("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (faceAuthProxy_ != nullptr) {
        return faceAuthProxy_;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        IAM_LOGE("failed to get systemAbilityManager.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->CheckSystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH);
    if (remoteObject == nullptr) {
        IAM_LOGE("failed to get remoteObject.");
        return nullptr;
    }

    faceAuthProxy_ = iface_cast<IFaceAuth>(remoteObject);
    if ((faceAuthProxy_ == nullptr) || (faceAuthProxy_->AsObject() == nullptr)) {
        IAM_LOGE("failed to get faceAuthProxy_");
        return nullptr;
    }

    faceAuthProxy_->AsObject()->AddDeathRecipient(this);
    return faceAuthProxy_;
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
