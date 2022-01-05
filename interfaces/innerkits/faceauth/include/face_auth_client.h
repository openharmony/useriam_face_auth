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

#ifndef FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_H
#define FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_H

#include <list>
#include "iface_auth.h"
#include "face_auth_death_recipient.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthClient {
public:
    void ResetFaceAuthProxy();

    static FaceAuthClient &GetInstance();

    int32_t Init();

    int32_t Release();

    int32_t Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback);

    int32_t Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback);

    int32_t CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback);

    int32_t CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback);

    int64_t PreEnroll();

    int32_t PostEnroll();

    int32_t Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback);

    std::list<int32_t> GetEnrolledFaceIDs(const int32_t userId);

    void ResetTimeout();

    int32_t GetRemainingNum();

    int64_t GetRemainingTime();

    int32_t GetAngleDim();

    void RegisterDeathRecipient(const sptr<OnFaceAuth> &callback);

private:
    bool GetFaceAuthProxy();

private:
    std::mutex mutex_;
    sptr<IFaceAuth> faceAuthProxy_;
    sptr<IRemoteObject::DeathRecipient> recipient_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_INNERKITS_INCLUDE_FACE_AUTH_H