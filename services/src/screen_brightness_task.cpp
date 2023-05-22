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

#include "screen_brightness_task.h"

#include <vector>

#include "display_power_mgr_client.h"
#include "sensor_agent.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "finite_state_machine.h"
#include "screen_brightness_manager.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
using ResultCode = UserAuth::ResultCode;
using namespace DisplayPowerMgr;
namespace {
constexpr SensorUser SENSOR_USER = {
    "FaceAuthService",
    [](SensorEvent *event) {
        IF_FALSE_LOGE_AND_RETURN(event != nullptr);
        IF_FALSE_LOGE_AND_RETURN(event->sensorTypeId == SENSOR_TYPE_ID_AMBIENT_LIGHT);
        auto data = static_cast<AmbientLightData *>(static_cast<void *>(event->data));
        IF_FALSE_LOGE_AND_RETURN(data != nullptr);

        auto manager = ScreenBrightnessManager::GetInstance();
        IF_FALSE_LOGE_AND_RETURN(manager != nullptr);
        auto task = manager->GetCurrentTask();
        IF_FALSE_LOGE_AND_RETURN(task != nullptr);

        task->SetAmbientLight(data->intensity);
    },
};
constexpr uint32_t INVALID_BRIGHTNESS = -1;
constexpr uint32_t SENSOR_SAMPLE_AND_REPORT_INTERVAL = 100 * 1000 * 1000; // ns
constexpr uint32_t INCREASE_BRIGHTNESS_START_DELAY = 100;                 // ms
constexpr uint32_t INCREASE_BRIGHTNESS_INTERVAL = 75;                     // ms
const std::vector<uint32_t> INCREASE_BRIGHTNESS_ARRAY = { 4, 4, 5, 5, 6, 6, 7, 8, 9, 9, 10, 11, 13, 14, 15, 17, 18, 20,
    22, 24, 27, 30, 33, 36, 39, 43, 48, 52, 58, 63, 70, 77, 84, 93, 102, 112, 124, 136, 150, 166, 181, 199, 219, 241 };
constexpr uint32_t INCREASE_BRIGHTNESS_MAX = 30;
constexpr float AMBIENT_LIGHT_THRESHOLD_FOR_BEGIN = 2.0; // lux
ResultCode SubscribeSensor()
{
    IAM_LOGI("start");
    int32_t subscribeSensorRet = SubscribeSensor(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER);
    IF_FALSE_LOGE_AND_RETURN_VAL(subscribeSensorRet == 0, ResultCode::GENERAL_ERROR);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER, SENSOR_SAMPLE_AND_REPORT_INTERVAL,
        SENSOR_SAMPLE_AND_REPORT_INTERVAL);
    IF_FALSE_LOGE_AND_RETURN_VAL(setBatchRet == 0, ResultCode::GENERAL_ERROR);
    int32_t activateSensorRet = ActivateSensor(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER);
    IF_FALSE_LOGE_AND_RETURN_VAL(activateSensorRet == 0, ResultCode::GENERAL_ERROR);
    int32_t setModeRet = SetMode(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER, SENSOR_ON_CHANGE);
    IF_FALSE_LOGE_AND_RETURN_VAL(setModeRet == 0, ResultCode::GENERAL_ERROR);

    return ResultCode::SUCCESS;
}
void UnsubscribeSensor()
{
    IAM_LOGI("start");
    DeactivateSensor(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER);
    UnsubscribeSensor(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER);
    return;
}

uint32_t GetIncreaseBrightnessStartIndex(uint32_t currentBrightness)
{
    for (uint32_t i = 0; i < INCREASE_BRIGHTNESS_ARRAY.size(); i++) {
        if (INCREASE_BRIGHTNESS_ARRAY[i] > currentBrightness) {
            return i;
        }
    }
    return INCREASE_BRIGHTNESS_ARRAY.size();
}

void OverrideScreenBrightness(uint32_t brightness)
{
    int32_t displayId = DisplayPowerMgrClient::GetInstance().GetMainDisplayId();
    if (!DisplayPowerMgrClient::GetInstance().OverrideBrightness(brightness, displayId)) {
        IAM_LOGE("override brightness fail");
        return;
    }
}

void RestoreScreenBrightness()
{
    int32_t displayId = DisplayPowerMgrClient::GetInstance().GetMainDisplayId();
    if (!DisplayPowerMgrClient::GetInstance().RestoreBrightness(displayId)) {
        IAM_LOGE("restore brightness fail");
        return;
    }
}

uint32_t GetCurrentScreenBrightness()
{
    return DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
}

bool ShouldBeginIncreaseBrightness(float currentAmbientLightLux)
{
    return currentAmbientLightLux < AMBIENT_LIGHT_THRESHOLD_FOR_BEGIN;
}
} // namespace

ScreenBrightnessTask::ScreenBrightnessTask() : timer_("screen_brightness_timer")
{
    timer_.Setup();
    increaseBrightnessIndex_ = 0;
}

ScreenBrightnessTask::~ScreenBrightnessTask()
{
    timer_.Shutdown();
}

void ScreenBrightnessTask::Start()
{
    machine_ = MakeFiniteStateMachine();
    IF_FALSE_LOGE_AND_RETURN(machine_ != nullptr);

    machine_->Schedule(E_START);
}

void ScreenBrightnessTask::Stop()
{
    IF_FALSE_LOGE_AND_RETURN(machine_ != nullptr);
    machine_->Schedule(E_STOP);
}

std::shared_ptr<FiniteStateMachine> ScreenBrightnessTask::MakeFiniteStateMachine()
{
    auto builder = FiniteStateMachine::Builder::New("ScreenBrightnessTask", State::S_INIT);
    IF_FALSE_LOGE_AND_RETURN_VAL(builder != nullptr, nullptr);

    // S_INIT
    builder->MakeTransition(S_INIT, E_START, S_WAIT_AMBIENT_LIGHT_INFO_AND_START_DELAY,
        [self = shared_from_this()](FiniteStateMachine &machine, uint32_t event) { self->StartProcess(); });

    // S_WAIT_*
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO_AND_START_DELAY, E_START_DELAY_EXPIRED,
        S_WAIT_AMBIENT_LIGHT_INFO);
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO_AND_START_DELAY, E_RECEIVE_AMBIENT_LIGHT_INFO,
        S_WAIT_START_DELAY);
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO, E_RECEIVE_AMBIENT_LIGHT_INFO, S_INCREASING_BRIGHTNESS);
    builder->MakeTransition(S_WAIT_START_DELAY, E_START_DELAY_EXPIRED, S_INCREASING_BRIGHTNESS);

    // S_INCREASING_BRIGHTNESS
    builder->MakeOnStateEnter(S_INCREASING_BRIGHTNESS,
        [self = shared_from_this()](FiniteStateMachine &machine, uint32_t event) { self->BeginIncreaseBrightness(); });

    builder->MakeTransition(S_INCREASING_BRIGHTNESS, E_INCREASE_BRIGHTNESS, S_INCREASING_BRIGHTNESS,
        [self = shared_from_this()](FiniteStateMachine &machine, uint32_t event) { self->DoIncreaseBrightness(); });

    // S_END
    builder->MakeOnStateEnter(S_END,
        [self = shared_from_this()](FiniteStateMachine &machine, uint32_t event) { self->EndProcess(); });

    // process E_STOP
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO_AND_START_DELAY, E_STOP, S_END);
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO, E_STOP, S_END);
    builder->MakeTransition(S_WAIT_START_DELAY, E_STOP, S_END);
    builder->MakeTransition(S_INCREASING_BRIGHTNESS, E_STOP, S_END);
    builder->MakeTransition(S_END, E_STOP, S_END);

    // ignore E_RECEIVE_AMBIENT_LIGHT_INFO
    builder->MakeTransition(S_WAIT_START_DELAY, E_RECEIVE_AMBIENT_LIGHT_INFO, S_WAIT_START_DELAY);
    builder->MakeTransition(S_INCREASING_BRIGHTNESS, E_RECEIVE_AMBIENT_LIGHT_INFO, S_INCREASING_BRIGHTNESS);
    builder->MakeTransition(S_END, E_RECEIVE_AMBIENT_LIGHT_INFO, S_END);
    return builder->Build();
}

void ScreenBrightnessTask::SetAmbientLight(float lux)
{
    IAM_LOGI("receive ambient light %{public}f", lux);
    {
        std::lock_guard<std::mutex> lock(currentAmbientLightLuxMutex_);
        currentAmbientLightLux_ = lux;
    }

    IF_FALSE_LOGE_AND_RETURN(machine_ != nullptr);
    machine_->Schedule(E_RECEIVE_AMBIENT_LIGHT_INFO);
}

void ScreenBrightnessTask::OnStartDelayTimeout()
{
    IAM_LOGI("start");
    IF_FALSE_LOGE_AND_RETURN(machine_ != nullptr);
    machine_->Schedule(E_START_DELAY_EXPIRED);
}

void ScreenBrightnessTask::OnIncreaseBrightness()
{
    IAM_LOGI("start");
    IF_FALSE_LOGE_AND_RETURN(machine_ != nullptr);
    machine_->Schedule(E_INCREASE_BRIGHTNESS);
}

void ScreenBrightnessTask::StartProcess()
{
    IAM_LOGI("start");
    IF_FALSE_LOGE_AND_RETURN(machine_ != nullptr);

    ResultCode result = SubscribeSensor();
    if (result != ResultCode::SUCCESS) {
        IAM_LOGE("SubscribeSensor fail");
        machine_->Schedule(E_STOP);
        return;
    }

    timer_.Unregister(currTimerId_);
    currTimerId_ = timer_.Register([self = shared_from_this()]() { self->OnStartDelayTimeout(); },
        INCREASE_BRIGHTNESS_START_DELAY, true);

    IAM_LOGI("time id %{public}d", currTimerId_);
}

void ScreenBrightnessTask::BeginIncreaseBrightness()
{
    IAM_LOGI("start");
    IF_FALSE_LOGE_AND_RETURN(machine_ != nullptr);

    bool shouldIncrease = false;
    {
        std::lock_guard<std::mutex> lock(currentAmbientLightLuxMutex_);
        shouldIncrease = ShouldBeginIncreaseBrightness(currentAmbientLightLux_);
    }
    if (!shouldIncrease) {
        IAM_LOGI("no need increase");
        machine_->Schedule(E_STOP);
        return;
    }

    currentBrightness_ = GetCurrentScreenBrightness();
    IF_FALSE_LOGE_AND_RETURN(currentBrightness_ != INVALID_BRIGHTNESS);
    increaseBrightnessIndex_ = GetIncreaseBrightnessStartIndex(currentBrightness_);
    IF_FALSE_LOGE_AND_RETURN(increaseBrightnessIndex_ < INCREASE_BRIGHTNESS_ARRAY.size());

    IAM_LOGI("current brightness %{public}u array index %{public}u value %{public}u", currentBrightness_,
        increaseBrightnessIndex_, INCREASE_BRIGHTNESS_ARRAY[increaseBrightnessIndex_]);
    DoIncreaseBrightness();
}

void ScreenBrightnessTask::DoIncreaseBrightness()
{
    IAM_LOGI("start");

    if (increaseBrightnessIndex_ < INCREASE_BRIGHTNESS_ARRAY.size()) {
        if (INCREASE_BRIGHTNESS_ARRAY[increaseBrightnessIndex_] <= INCREASE_BRIGHTNESS_MAX) {
            OverrideScreenBrightness(INCREASE_BRIGHTNESS_ARRAY[increaseBrightnessIndex_]);
            IAM_LOGI("increase brightness index %{public}u value %{public}u", increaseBrightnessIndex_,
                INCREASE_BRIGHTNESS_ARRAY[increaseBrightnessIndex_]);
            timer_.Unregister(currTimerId_);
            currTimerId_ = timer_.Register([self = shared_from_this()]() { self->OnIncreaseBrightness(); },
                INCREASE_BRIGHTNESS_INTERVAL, true);
        }
        increaseBrightnessIndex_++;
    }
}

void ScreenBrightnessTask::EndProcess()
{
    timer_.Unregister(currTimerId_);
    UnsubscribeSensor();

    RestoreScreenBrightness();
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS