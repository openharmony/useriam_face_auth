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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_EVENT_HANDLER_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_EVENT_HANDLER_H

#include "event_handler.h"
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthEventHandler : public AppExecFwk::EventHandler {
public:
    FaceAuthEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~FaceAuthEventHandler();
    void RemoveEvent(const uint64_t reqId);
    template <typename T>
    inline bool SendEvent(int32_t innerEventId, std::unique_ptr<T> &&object, Priority priority)
    {
        return AppExecFwk::EventHandler::SendEvent(AppExecFwk::InnerEvent::Get((uint32_t) innerEventId, object), 0,
            priority);
    }
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event);
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_EVENT_HANDLER_H
