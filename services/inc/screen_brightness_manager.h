/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SCREEN_BRIGHTNESS_MANAGER
#define SCREEN_BRIGHTNESS_MANAGER

#include <map>
#include <mutex>

#include "nocopyable.h"

#include "face_auth_executor_hdi.h"
#include "face_auth_hdi.h"
#include "isa_command_processor.h"
#include "iscreen_brightness_task.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
class ScreenBrightnessManager : public std::enable_shared_from_this<ScreenBrightnessManager>,
                                public ISaCommandProcessor,
                                public NoCopyable {
public:
    ScreenBrightnessManager() = default;
    ~ScreenBrightnessManager() override = default;
    static std::shared_ptr<ScreenBrightnessManager> GetInstance();

    UserAuth::ResultCode ProcessSaCommand(std::shared_ptr<FaceAuthExecutorHdi> executor,
        const SaCommand &command) override;
    void OnHdiDisconnect(std::shared_ptr<FaceAuthExecutorHdi> executor) override;

    std::shared_ptr<IScreenBrightnessTask> GetCurrentTask();

private:
    UserAuth::ResultCode ProcessScreenBrightnessIncreaseBegin(std::shared_ptr<FaceAuthExecutorHdi> executor,
        const SaCommandParam param);
    UserAuth::ResultCode ProcessScreenBrightnessIncreaseEnd(std::shared_ptr<FaceAuthExecutorHdi> executor,
        const SaCommandParam param);

    std::shared_ptr<FaceAuthExecutorHdi> executorInProc_ = nullptr;
    std::shared_ptr<IScreenBrightnessTask> taskInProc_ = nullptr;
    std::mutex mutex_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // SCREEN_BRIGHTNESS_MANAGER