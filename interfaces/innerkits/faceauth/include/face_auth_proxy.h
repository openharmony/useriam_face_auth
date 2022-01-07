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

#ifndef FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_PROXY_H
#define FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_PROXY_H
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
    virtual int32_t Init() override;
    virtual int32_t Release() override;
    virtual int32_t Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback) override;
    virtual int32_t Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback) override;
    virtual int32_t CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback) override;
    virtual int32_t CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback) override;
    virtual int64_t PreEnroll() override;
    virtual int32_t PostEnroll() override;
    virtual int32_t Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback) override;
    virtual std::list<int32_t> GetEnrolledFaceIDs(const int32_t userId = 0) override;
    virtual void ResetTimeout() override;
    virtual int32_t GetRemainingNum() override;
    virtual int32_t GetAngleDim() override;
    virtual int64_t GetRemainingTime() override;

private:
    bool SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply);

private:
    static inline BrokerDelegator<FaceAuthProxy> delegator_;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_PROXY_H