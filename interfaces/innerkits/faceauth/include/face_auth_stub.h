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

#ifndef FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_STUB_H
#define FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_STUB_H

#include <map>
#include "faceauth_log_wrapper.h"
#include "iface_auth.h"
#include "iremote_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthStub : public IRemoteStub<IFaceAuth> {
public:
    FaceAuthStub();
    virtual ~FaceAuthStub() override;

    virtual int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using PHandle = int32_t (FaceAuthStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, PHandle> m_KeyToHandle_;
    DISALLOW_COPY_AND_MOVE(FaceAuthStub);
    void MyKeyToHandle();
    int32_t FaceAuthAuthenticate(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthCancelAuth(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthInit(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthRelease(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthResetTimeout(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthGetRemainingNum(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthGetRemainingTime(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthPreEnroll(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthPostEnroll(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthRemove(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthGetEnrolledFaceIds(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthGetAngledim(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthEnroll(MessageParcel &data, MessageParcel &reply);
    int32_t FaceAuthCancelEnrollment(MessageParcel &data, MessageParcel &reply);
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_STUB_H