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

#ifndef ON_FACEAUTH_H
#define ON_FACEAUTH_H

#include <string>

#include "errors.h"
#include "iremote_broker.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
struct Bundle {
    std::vector<uint8_t> authtoken;
};
struct Tip {
    int32_t errorCode;
};
class OnFaceAuth : public IRemoteBroker {
public:
    enum {
        ON_CALLBACK_EVENT = 0,
        ON_DEATH_RECIPIENT = 1,
    };

public:
    virtual void OnCallbackEvent(uint64_t reqId, int32_t type, int32_t code, int32_t errorCode, Bundle &bundle) = 0;

    virtual void OnDeathRecipient() = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.security.FaceAuth.OnFaceAuth");
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // ON_FACEAUTH_H
