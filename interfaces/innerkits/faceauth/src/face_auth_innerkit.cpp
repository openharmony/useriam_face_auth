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

#include "face_auth_innerkit.h"
#include "faceauth_log_wrapper.h"
#include "face_auth_client.h"
#include "singleton.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
int32_t FaceAuthInnerKit::Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("Authenticate enter");
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE(" Authenticate callback is null!");
        return FA_RET_ERROR;
    }

    return FaceAuthClient::GetInstance().Authenticate(param, callback);
}

int32_t FaceAuthInnerKit::CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("CancelAuth enter");
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE(" CancelAuth callback is null!");
        return FA_RET_ERROR;
    }

    return FaceAuthClient::GetInstance().CancelAuth(reqId, callback);
}

int32_t FaceAuthInnerKit::Init()
{
    return FaceAuthClient::GetInstance().Init();
}

int32_t FaceAuthInnerKit::Release()
{
    return FaceAuthClient::GetInstance().Release();
}

void FaceAuthInnerKit::ResetTimeout()
{
    FaceAuthClient::GetInstance().ResetTimeout();
    return;
}

int64_t FaceAuthInnerKit::GetRemainingTime()
{
    return FaceAuthClient::GetInstance().GetRemainingTime();
}

int32_t FaceAuthInnerKit::GetRemainingNum()
{
    return FaceAuthClient::GetInstance().GetRemainingNum();
}

int32_t FaceAuthInnerKit::Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("Enroll enter");
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE(" Enroll callback is null!");
        return FA_RET_ERROR;
    }

    return FaceAuthClient::GetInstance().Enroll(param, callback);
}

int32_t FaceAuthInnerKit::CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("CancelEnrollment enter");
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE(" CancelEnrollment callback is null!");
        return FA_RET_ERROR;
    }

    return FaceAuthClient::GetInstance().CancelEnrollment(reqId, callback);
}

int64_t FaceAuthInnerKit::PreEnroll()
{
    return FaceAuthClient::GetInstance().PreEnroll();
}

int32_t FaceAuthInnerKit::PostEnroll()
{
    return FaceAuthClient::GetInstance().PostEnroll();
}

int32_t FaceAuthInnerKit::Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("Remove enter");
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE("Remove callback is null!");
        return FA_RET_ERROR;
    }

    return FaceAuthClient::GetInstance().Remove(param, callback);
}

std::list<int32_t> FaceAuthInnerKit::GetEnrolledFaceIDs(const int32_t userId)
{
    return FaceAuthClient::GetInstance().GetEnrolledFaceIDs(userId);
}

int32_t FaceAuthInnerKit::GetAngleDim()
{
    return FaceAuthClient::GetInstance().GetAngleDim();
}

void FaceAuthInnerKit::RegisterDeathRecipient(const sptr<OnFaceAuth> &callback)
{
    FaceAuthClient::GetInstance().RegisterDeathRecipient(callback);
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
