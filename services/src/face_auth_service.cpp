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

#include "face_auth_service.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "ibuffer_producer.h"
#include "idriver_manager.h"
#include "iremote_object.h"
#include "refbase.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_para2str.h"
#include "iam_ptr.h"

#include "face_auth_defines.h"
#include "face_auth_driver_hdi.h"
#include "face_auth_interface_adapter.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(FaceAuthService::GetInstance().get());
std::mutex FaceAuthService::mutex_;
std::shared_ptr<FaceAuthService> FaceAuthService::instance_ = nullptr;

FaceAuthService::FaceAuthService() : SystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH, true)
{
}

std::shared_ptr<FaceAuthService> FaceAuthService::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> gurard(mutex_);
        if (instance_ == nullptr) {
            instance_ = Common::MakeShared<FaceAuthService>();
            if (instance_ == nullptr) {
                IAM_LOGE("make share failed");
            }
        }
    }
    return instance_;
}

void FaceAuthService::OnStart()
{
    IAM_LOGI("start");
    StartDriverManager();
    Publish(this);
    IAM_LOGI("success");
}

void FaceAuthService::OnStop()
{
    IAM_LOGE("service is persistent, OnStop is not implemented");
}

int32_t FaceAuthService::SetBufferProducer(sptr<IBufferProducer> &producer)
{
    std::lock_guard<std::mutex> gurard(mutex_);
    IAM_LOGI("set buffer producer %{public}s", Common::GetPointerNullStateString(producer).c_str());
    bufferProducer_ = producer;
    return FACEAUTH_SUCCESS;
}

sptr<IBufferProducer> FaceAuthService::FaceAuthService::GetBufferProducer()
{
    std::lock_guard<std::mutex> gurard(mutex_);
    IAM_LOGI("get buffer producer %{public}s", Common::GetPointerNullStateString(bufferProducer_).c_str());
    return bufferProducer_;
}

void FaceAuthService::StartDriverManager()
{
    IAM_LOGI("start");
    auto adapter = Common::MakeShared<FaceAuthInterfaceAdapter>();
    IF_FALSE_LOGE_AND_RETURN(adapter != nullptr);
    auto faceAuthDefaultHdi = Common::MakeShared<FaceAuthDriverHdi>(adapter);
    IF_FALSE_LOGE_AND_RETURN(faceAuthDefaultHdi != nullptr);
    const uint16_t faceAuthDefaultHdiId = 1;
    // serviceName and HdiConfig.id must be globally unique
    const std::map<std::string, UserAuth::HdiConfig> hdiName2Config = {
        {"face_auth_interface_service", {faceAuthDefaultHdiId, faceAuthDefaultHdi}},
    };
    int32_t ret = UserAuth::IDriverManager::Start(hdiName2Config);
    if (ret != FACEAUTH_SUCCESS) {
        IAM_LOGE("start driver manager failed");
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
