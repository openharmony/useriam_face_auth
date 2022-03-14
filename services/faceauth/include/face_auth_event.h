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

#ifndef FACE_AUTH_EVENT_H
#define FACE_AUTH_EVENT_H

#include <mutex>
#include "face_auth_defines.h"
#include "face_auth_event_handler.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthEvent {
public:
    FaceAuthEvent();
    virtual ~FaceAuthEvent();
    static std::shared_ptr<FaceAuthEvent> GetInstance();
    void HandleTask(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessAuthenticateTask(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessEnrollTask(const AppExecFwk::InnerEvent::Pointer &event);
    void ProcessRemoveTask(const AppExecFwk::InnerEvent::Pointer &event);
    inline void SetEventHandler(const std::shared_ptr<FaceAuthEventHandler> &handler)
    {
        eventHandler_ = handler;
    }
private:
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthEvent> instance_;
    std::shared_ptr<FaceAuthEventHandler> eventHandler_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // FACE_AUTH_EVENT_H
