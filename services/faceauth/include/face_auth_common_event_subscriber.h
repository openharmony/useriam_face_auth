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

#ifndef FACE_AUTH_COMMON_EVENT_SUBSCRIBER_H
#define FACE_AUTH_COMMON_EVENT_SUBSCRIBER_H

#include "face_auth_service.h"
#include "common_event_subscriber.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
using CommonEventSubscriber = OHOS::EventFwk::CommonEventSubscriber;
using CommonEventData = OHOS::EventFwk::CommonEventData;
using CommonEventSubscribeInfo = OHOS::EventFwk::CommonEventSubscribeInfo;

class FaceAuthCommonEventSubscriber : public CommonEventSubscriber {
public:
    explicit FaceAuthCommonEventSubscriber(
        const CommonEventSubscribeInfo &subscribeInfo, FaceAuthService* callback);
    ~FaceAuthCommonEventSubscriber() = default;

    void OnReceiveEvent(const CommonEventData &data) override;

private:
    FaceAuthService* callback_;
};
} // namespace FaceAuth
} // namespace UserIAM
}  // namespace OHOS

#endif  // FACE_AUTH_COMMON_EVENT_SUBSCRIBER_H
