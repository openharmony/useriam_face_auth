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

#include "face_auth_client.h"
#include "faceauth_log_wrapper.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthClient &FaceAuthClient::GetInstance()
{
    static FaceAuthClient instance;
    return instance;
}

int32_t FaceAuthClient::Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->Authenticate(param, callback);
    return ret;
}

int32_t FaceAuthClient::CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->CancelAuth(reqId, callback);
    return ret;
}

int32_t FaceAuthClient::Init()
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->Init();
    return ret;
}

int32_t FaceAuthClient::Release()
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->Release();
    return ret;
}

void FaceAuthClient::ResetTimeout()
{
    if (!GetFaceAuthProxy()) {
        return;
    }
    faceAuthProxy_->ResetTimeout();
    return;
}

int32_t FaceAuthClient::GetRemainingNum()
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->GetRemainingNum();
    return ret;
}

int64_t FaceAuthClient::GetRemainingTime()
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int64_t ret = faceAuthProxy_->GetRemainingTime();
    return ret;
}

void FaceAuthClient::ResetFaceAuthProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if ((faceAuthProxy_ != nullptr) && (faceAuthProxy_->AsObject() != nullptr)) {
        faceAuthProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    faceAuthProxy_ = nullptr;
    return;
}

bool FaceAuthClient::GetFaceAuthProxy()
{
    FACEAUTH_LABEL_LOGI(" GetFaceAuthProxy  enter");
    if (!faceAuthProxy_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (faceAuthProxy_) {
            return true;
        }
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            FACEAUTH_LABEL_LOGE("failed to get systemAbilityManager.");
            return false;
        }
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH);
        if (!remoteObject) {
            FACEAUTH_LABEL_LOGE("failed to get remoteObject.");
            return false;
        }

        faceAuthProxy_ = iface_cast<IFaceAuth>(remoteObject);
        if ((!faceAuthProxy_) || (!faceAuthProxy_->AsObject())) {
            FACEAUTH_LABEL_LOGE("failed to get faceAuthProxy_.");
            return false;
        }

        if (!recipient_) {
            recipient_ = new FaceAuthDeathRecipient();
        }
        if (!recipient_) {
            FACEAUTH_LABEL_LOGE("failed to new recipient_.");
            return false;
        }
        faceAuthProxy_->AsObject()->AddDeathRecipient(recipient_);
    }
    return true;
}

int32_t FaceAuthClient::CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->CancelEnrollment(reqId, callback);
    return ret;
}

int64_t FaceAuthClient::PreEnroll()
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int64_t ret = faceAuthProxy_->PreEnroll();
    return ret;
}

int32_t FaceAuthClient::PostEnroll()
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->PostEnroll();
    return ret;
}

int32_t FaceAuthClient::Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback)
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->Remove(param, callback);
    return ret;
}

int32_t FaceAuthClient::GetAngleDim()
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->GetAngleDim();
    return ret;
}

std::list<int32_t> FaceAuthClient::GetEnrolledFaceIDs(const int32_t userId)
{
    std::list<int32_t> list;
    if (!GetFaceAuthProxy()) {
        return list;
    }
    list = faceAuthProxy_->GetEnrolledFaceIDs(userId);
    return list;
}

int32_t FaceAuthClient::Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback)
{
    if (!GetFaceAuthProxy()) {
        return FA_RET_ERROR;
    }
    int32_t ret = faceAuthProxy_->Enroll(param, callback);
    return ret;
}

void FaceAuthClient::RegisterDeathRecipient(const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("Register death recipient.");
    recipient_ = new FaceAuthDeathRecipient(callback);
    if (!recipient_) {
        FACEAUTH_LABEL_LOGE("failed to new recipient_.");
        return;
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
