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

#ifndef FACE_AUTH_INNER_kit_H
#define FACE_AUTH_INNER_kit_H

#include <list>
#include <iostream>
#include "face_auth_defines.h"
#include "constant.h"
#include "on_faceauth.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthInnerKit {
public:
    static int32_t Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback);

    static int32_t CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback);

    static int32_t Init();

    static int32_t Release();

    static void ResetTimeout();

    static int64_t GetRemainingTime();

    static int32_t GetRemainingNum();

    static int32_t Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback);

    static int32_t CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback);

    static int64_t PreEnroll();

    static int32_t PostEnroll();

    static int32_t Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback);

    static std::list<int32_t> GetEnrolledFaceIDs(const int32_t userId);

    static int32_t GetAngleDim();

    static void RegisterDeathRecipient(const sptr<OnFaceAuth> &callback);

private:
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // FACE_AUTH_INNER_kit_H