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

#include "face_auth_event_handler.h"
#include "face_auth_log_wrapper.h"
#include "face_auth_defines.h"
#include "face_auth_event.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthEventHandler::FaceAuthEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner)
{}

FaceAuthEventHandler::~FaceAuthEventHandler()
{}

void FaceAuthEventHandler::RemoveEvent(const uint64_t reqId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "remove event %{public}s", getMaskedString(reqId).c_str());
    EventHandler::RemoveEvent(reqId);
    return;
}

void FaceAuthEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "event is nullptr");
        return;
    }
    FaceAuthEvent::GetInstance()->HandleTask(event);
    return;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
