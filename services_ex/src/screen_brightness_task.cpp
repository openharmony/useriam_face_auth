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

#include <string>
#include <vector>

#ifdef FACE_USE_DISPLAY_MANAGER_COMPONENT
#include "display_power_mgr_client.h"
#endif
#include "parameter.h"
#ifdef FACE_USE_SENSOR_COMPONENT
#include "sensor_agent.h"
#endif

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "finite_state_machine.h"
#include "screen_brightness_manager.h"

#define LOG_TAG "FACE_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
using ResultCode = UserAuth::ResultCode;
#ifdef FACE_USE_DISPLAY_MANAGER_COMPONENT
using namespace DisplayPowerMgr;
#endif
namespace {
#ifdef FACE_USE_SENSOR_COMPONENT
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
#endif
constexpr uint32_t INVALID_BRIGHTNESS = -1;
constexpr uint32_t SENSOR_SAMPLE_AND_REPORT_INTERVAL = 100 * 1000 * 1000; // ns
constexpr uint32_t INCREASE_BRIGHTNESS_START_DELAY = 100;                 // ms
constexpr uint32_t DEFAULT_INCREASE_BRIGHTNESS_INTERVAL = 75;             // ms
constexpr uint32_t DEFAULT_INCREASE_BRIGHTNESS_MAX = 30;
const std::vector<uint32_t> INCREASE_BRIGHTNESS_ARRAY = { 4, 4, 5, 5, 6, 6, 7, 8, 9, 9, 10, 11, 13, 14, 15, 17, 18, 20,
    22, 24, 27, 30, 33, 36, 39, 43, 48, 52, 58, 63, 70, 77, 84, 93, 102, 112, 124, 136, 150, 166, 181, 199, 219, 241 };
constexpr float AMBIENT_LIGHT_THRESHOLD_FOR_BEGIN = 2.0; // lux
constexpr uint32_t MAX_BRIGHTNESS = 255;
constexpr uint32_t MAX_INT_STRING_LEN = 12;
constexpr float INVALID_AMBIENT_LIGHT_LUX = -1.0;
const char *INCREASE_BRIGHTNESS_MAX_KEY = "const.useriam.face_auth_increase_brightness_max";
const char *INCREASE_BRIGHTNESS_INTERVAL_KEY = "const.useriam.face_auth_increase_brightness_interval";

uint32_t GetUInt32Param(const char *key, uint32_t defaultValue)
{
    std::string defaultStr = std::to_string(defaultValue);
    char str[MAX_INT_STRING_LEN] = { 0 };
    int32_t ret = GetParameter(key, defaultStr.c_str(), str, MAX_INT_STRING_LEN - 1);
    if (ret < 0) {
        IAM_LOGE("failed to get param %{public}s, return default value", key);
        return defaultValue;
    }
    uint32_t uintValue;
    try {
        unsigned long longValue = std::stoul(str);
        if (longValue > std::numeric_limits<uint32_t>::max()) {
            IAM_LOGE("value exceeds uint32");
            return std::numeric_limits<uint32_t>::max();
        }
        uintValue = static_cast<uint32_t>(longValue);
    } catch (const std::exception &e) {
        IAM_LOGE("failed to convert %{public}s to int, return default value", str);
        return defaultValue;
    }

    return uintValue;
}

uint32_t GetIncreaseBrightnessInterval()
{
    uint32_t val = GetUInt32Param(INCREASE_BRIGHTNESS_INTERVAL_KEY, DEFAULT_INCREASE_BRIGHTNESS_INTERVAL);
    if (val == 0) {
        IAM_LOGE("interval cannot be 0");
        return DEFAULT_INCREASE_BRIGHTNESS_INTERVAL;
    }
    IAM_LOGI("param interval %{public}u", val);
    return val;
}

uint32_t GetIncreaseBrightnessMax()
{
    uint32_t val = GetUInt32Param(INCREASE_BRIGHTNESS_MAX_KEY, DEFAULT_INCREASE_BRIGHTNESS_MAX);
    if (val > MAX_BRIGHTNESS) {
        IAM_LOGE("val exceeds max brightness");
        return MAX_BRIGHTNESS;
    }
    IAM_LOGI("param increase brightness max %{public}u", val);
    return val;
}

ResultCode SubscribeSensor()
{
    IAM_LOGI("start");
#ifdef FACE_USE_SENSOR_COMPONENT
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
#else
    IAM_LOGI("sensor component is not used");
    return ResultCode::GENERAL_ERROR
#endif
}

void UnsubscribeSensor()
{
    IAM_LOGI("start");
#ifdef FACE_USE_SENSOR_COMPONENT
    DeactivateSensor(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER);
    UnsubscribeSensor(SENSOR_TYPE_ID_AMBIENT_LIGHT, &SENSOR_USER);
    return;
#else
    IAM_LOGI("sensor component is not used");
    return;
#endif
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
#ifdef FACE_USE_DISPLAY_MANAGER_COMPONENT
    int32_t displayId = DisplayPowerMgrClient::GetInstance().GetMainDisplayId();
    if (!DisplayPowerMgrClient::GetInstance().OverrideBrightness(brightness, displayId)) {
        IAM_LOGE("override brightness fail");
        return;
    }
#else
    return;
#endif
}

void RestoreScreenBrightness()
{
#ifdef FACE_USE_DISPLAY_MANAGER_COMPONENT
    int32_t displayId = DisplayPowerMgrClient::GetInstance().GetMainDisplayId();
    if (!DisplayPowerMgrClient::GetInstance().RestoreBrightness(displayId)) {
        IAM_LOGE("restore brightness fail");
        return;
    }
#else
    return;
#endif
}

uint32_t GetCurrentScreenBrightness()
{
#ifdef FACE_USE_DISPLAY_MANAGER_COMPONENT
    return DisplayPowerMgrClient::GetInstance().GetDeviceBrightness();
#else
    IAM_LOGI("display_manager component is not used.");
    return INVALID_BRIGHTNESS;
#endif
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
    increaseBrightnessInterval_ = GetIncreaseBrightnessInterval();
    increaseBrightnessMax_ = GetIncreaseBrightnessMax();
    currTimerId_ = 0;
    currentBrightness_ = 0;
    currentAmbientLightLux_ = INVALID_AMBIENT_LIGHT_LUX;
}

ScreenBrightnessTask::~ScreenBrightnessTask()
{
    timer_.Unregister(currTimerId_);
    timer_.Shutdown();
    if (machine_ != nullptr) {
        // make sure state machine is stopped
        uint32_t state = machine_->EnsureCurrentState();
        if (state != S_END) {
            IAM_LOGE("state is not STOP when destruct");
        }
    }
    if (destructCallback_ != nullptr) {
        destructCallback_();
    }
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
        [self = this](FiniteStateMachine &machine, uint32_t event) { self->StartProcess(); });

    // S_WAIT_*
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO_AND_START_DELAY, E_START_DELAY_EXPIRED,
        S_WAIT_AMBIENT_LIGHT_INFO);
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO_AND_START_DELAY, E_RECEIVE_AMBIENT_LIGHT_INFO,
        S_WAIT_START_DELAY);
    builder->MakeTransition(S_WAIT_AMBIENT_LIGHT_INFO, E_RECEIVE_AMBIENT_LIGHT_INFO, S_INCREASING_BRIGHTNESS);
    builder->MakeTransition(S_WAIT_START_DELAY, E_START_DELAY_EXPIRED, S_INCREASING_BRIGHTNESS);

    // S_INCREASING_BRIGHTNESS
    builder->MakeOnStateEnter(S_INCREASING_BRIGHTNESS,
        [self = this](FiniteStateMachine &machine, uint32_t event) { self->BeginIncreaseBrightness(); });

    builder->MakeTransition(S_INCREASING_BRIGHTNESS, E_INCREASE_BRIGHTNESS, S_INCREASING_BRIGHTNESS,
        [self = this](FiniteStateMachine &machine, uint32_t event) { self->DoIncreaseBrightness(); });

    // S_END
    builder->MakeOnStateEnter(S_END,
        [self = this](FiniteStateMachine &machine, uint32_t event) { self->EndProcess(); });

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
    currTimerId_ = timer_.Register(
        [weak_self = weak_from_this()]() {
            auto self = weak_self.lock();
            if (self == nullptr) {
                IAM_LOGE("object is released");
                return;
            }
            self->OnStartDelayTimeout();
        },
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

    auto timeCallback = [weak_self = weak_from_this()]() {
        auto self = weak_self.lock();
        if (self == nullptr) {
            IAM_LOGE("object is released");
            return;
        }
        self->OnIncreaseBrightness();
    };

    if (increaseBrightnessIndex_ < INCREASE_BRIGHTNESS_ARRAY.size()) {
        if (INCREASE_BRIGHTNESS_ARRAY[increaseBrightnessIndex_] <= increaseBrightnessMax_) {
            OverrideScreenBrightness(INCREASE_BRIGHTNESS_ARRAY[increaseBrightnessIndex_]);
            IAM_LOGI("increase brightness index %{public}u value %{public}u", increaseBrightnessIndex_,
                INCREASE_BRIGHTNESS_ARRAY[increaseBrightnessIndex_]);
            timer_.Unregister(currTimerId_);
            currTimerId_ = timer_.Register(timeCallback, increaseBrightnessInterval_, true);
        }
        increaseBrightnessIndex_++;
    }
}

void ScreenBrightnessTask::EndProcess()
{
    IAM_LOGI("start");
    timer_.Unregister(currTimerId_);
    UnsubscribeSensor();
    RestoreScreenBrightness();
}

void ScreenBrightnessTask::RegisterDestructCallback(DestructCallback destructCallback)
{
    destructCallback_ = destructCallback;
}

__attribute__((visibility("default"))) std::shared_ptr<IScreenBrightnessTask> GetScreenBrightnessTask()
{
    return Common::MakeShared<ScreenBrightnessTask>();
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS