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

#ifndef FACE_AUTH_PROXY_H
#define FACE_AUTH_PROXY_H

#include <list>
#include "iface_auth.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthProxy : public IRemoteProxy<IFaceAuth> {
public:
    explicit FaceAuthProxy(const sptr<IRemoteObject> &object);
    virtual ~FaceAuthProxy() override;
    virtual int32_t SetBufferProducer(sptr<IBufferProducer> &producer) override;

private:
    bool SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply);
    static inline BrokerDelegator<FaceAuthProxy> delegator_;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACE_AUTH_PROXY_H