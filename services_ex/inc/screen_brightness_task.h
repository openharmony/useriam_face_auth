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

#ifndef SCREEN_BRIGHTNESS_TASK
#define SCREEN_BRIGHTNESS_TASK

#include <map>
#include <mutex>

#include "nocopyable.h"
#include "timer.h"

#include "finite_state_machine.h"
#include "iscreen_brightness_task.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class ScreenBrightnessTask : public IScreenBrightnessTask,
                             public std::enable_shared_from_this<ScreenBrightnessTask>,
                             public NoCopyable {
public:
    ScreenBrightnessTask();
    ~ScreenBrightnessTask() override;

    void Start() override;
    void Stop() override;
    void SetAmbientLight(float lux) override;
    void RegisterDestructCallback(DestructCallback callback) override;

    void OnStartDelayTimeout();
    void OnIncreaseBrightness();

private:
    enum State : uint32_t {
        S_INIT = 0,
        S_WAIT_AMBIENT_LIGHT_INFO_AND_START_DELAY = 1,
        S_WAIT_AMBIENT_LIGHT_INFO = 2,
        S_WAIT_START_DELAY = 3,
        S_INCREASING_BRIGHTNESS = 4,
        S_END = 5
    };
    enum Event : uint32_t {
        E_START = 0,
        E_START_DELAY_EXPIRED = 1,
        E_RECEIVE_AMBIENT_LIGHT_INFO = 2,
        E_INCREASE_BRIGHTNESS = 3,
        E_STOP = 4,
    };

    std::shared_ptr<FiniteStateMachine> MakeFiniteStateMachine();
    void StartProcess();
    void BeginIncreaseBrightness();
    void DoIncreaseBrightness();
    void EndProcess();

    std::shared_ptr<FiniteStateMachine> machine_;
    Utils::Timer timer_;
    uint32_t currTimerId_;
    float currentAmbientLightLux_;
    std::mutex currentAmbientLightLuxMutex_;
    uint32_t currentBrightness_;
    uint32_t increaseBrightnessIndex_;
    uint32_t increaseBrightnessInterval_;
    uint32_t increaseBrightnessMax_;
    DestructCallback destructCallback_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // SCREEN_BRIGHTNESS_TASK