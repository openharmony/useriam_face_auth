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

#ifndef FACE_AUTH_CLIENT_H
#define FACE_AUTH_CLIENT_H

#include <list>
#include "nocopyable.h"
#include "singleton.h"
#include "iface_auth.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthClient : public IRemoteObject::DeathRecipient {
    DECLARE_DELAYED_SINGLETON(FaceAuthClient);
    DISALLOW_COPY_AND_MOVE(FaceAuthClient);

public:
    void ResetFaceAuthProxy();
    static std::shared_ptr<FaceAuthClient> GetInstance();
    int32_t SetBufferProducer(sptr<IBufferProducer> &producer);
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    sptr<IFaceAuth> GetFaceAuthProxy();
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthClient> instance_;
    sptr<IFaceAuth> faceAuthProxy_;
    sptr<IRemoteObject::DeathRecipient> recipient_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACE_AUTH_CLIENT_H