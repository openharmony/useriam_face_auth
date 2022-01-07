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

#ifndef ON_FACEAUTH_PROXY_H
#define ON_FACEAUTH_PROXY_H
#include "iremote_proxy.h"
#include "nocopyable.h"
#include "on_faceauth.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class OnFaceAuthProxy : public IRemoteProxy<OnFaceAuth> {
public:
    explicit OnFaceAuthProxy(const sptr<IRemoteObject> &object);

    ~OnFaceAuthProxy() override;

    virtual void OnCallbackEvent(
        uint64_t reqId, int32_t type, int32_t code, int32_t errorCode, Bundle &Bundle) override;

    virtual void OnDeathRecipient() override;

private:
    DISALLOW_COPY_AND_MOVE(OnFaceAuthProxy);

    static inline BrokerDelegator<OnFaceAuthProxy> delegator_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // ON_FACEAUTH_PROXY_H
