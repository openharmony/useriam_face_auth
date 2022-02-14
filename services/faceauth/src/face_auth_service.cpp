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

#include "face_auth_service.h"
#include "face_auth_log_wrapper.h"
#include "face_auth_defines.h"
#include "common_event_manager.h"
#include "face_auth_common_event_subscriber.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const std::string REGISTER_NOTIFICATION = "EXECUTOR_REGISTER_NOTIFICATION";
std::mutex FaceAuthService::mutex_;
std::shared_ptr<FaceAuthService> FaceAuthService::instance_ = nullptr;
std::shared_ptr<FaceAuthManager> FaceAuthService::manager_ = nullptr;
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<FaceAuthService>::GetInstance().get());


FaceAuthService::FaceAuthService()
    : SystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH, false)
{}

FaceAuthService::~FaceAuthService()
{}

void FaceAuthService::OnStart()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthService Start");
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(REGISTER_NOTIFICATION);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<FaceAuthCommonEventSubscriber> subscriberPtr =
        std::make_shared<FaceAuthCommonEventSubscriber>(subscriberInfo, this);
    if (subscriberPtr != nullptr) {
        bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberPtr);
        if (!subscribeResult) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "SubscribeCommonEvent failed");
        }
    }
    Start();
}

void FaceAuthService::OnStop()
{
    if (manager_ != nullptr) {
        manager_->Release();
        manager_.reset();
    }
}
void FaceAuthService::Start()
{
    int32_t iRet = FA_RET_ERROR;
    manager_ = FaceAuthManager::GetInstance();
    if (manager_ != nullptr) {
        iRet = manager_->Init();
    }
    if (FA_RET_OK == iRet) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Init() result success.");
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Init() result failed.");
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
