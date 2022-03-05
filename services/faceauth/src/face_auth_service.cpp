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
#include "parameter.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const std::string REGISTER_NOTIFICATION = "EXECUTOR_REGISTER_NOTIFICATION";
FaceAuthService *FaceAuthService::instance_ = nullptr;
std::shared_ptr<FaceAuthManager> FaceAuthService::manager_ = nullptr;
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<FaceAuthService>::GetInstance().get());
static const char IAM_EVENT_KEY[] = "bootevent.useriam.fwkready";


FaceAuthService::FaceAuthService()
    : SystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH, false)
{
    instance_ = this;
}

FaceAuthService::~FaceAuthService()
{
    instance_ = nullptr;
}

FaceAuthService *FaceAuthService::GetInstance()
{
    return FaceAuthService::instance_;
}

static void UserIamBootEventCallback(const char *key, const char *value, void *context)
{
    FACEAUTH_HILOGD(MODULE_SERVICE, "UserIam is ready");
    if (key == nullptr || value == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "param is null");
        return;
    }
    if (strcmp(key, IAM_EVENT_KEY) || strcmp(value, "true")) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "event is mismatch");
        return;
    }
    FaceAuthService *faceService = FaceAuthService::GetInstance();
    if (faceService == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceService is null");
        return;
    }
    faceService->ReRegister();
}

void FaceAuthService::OnStart()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthService Start");
    WatchParameter(IAM_EVENT_KEY, UserIamBootEventCallback, nullptr);
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
    int32_t ret = FA_RET_ERROR;
    manager_ = FaceAuthManager::GetInstance();
    if (manager_ != nullptr) {
        ret = manager_->Init();
    }
    if (FA_RET_OK == ret) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Init() result success.");
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Init() result failed.");
    }
}

void FaceAuthService::ReRegister()
{
    if (manager_ == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "manager_ is null.");
        return;
    }
    manager_->QueryRegStatus();
}

} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
