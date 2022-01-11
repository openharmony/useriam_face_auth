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

#include "face_auth_service.h"
#include "constant.h"
#include "faceauth_log_wrapper.h"
#include "face_auth_defines.h"
#include "face_auth_manager.h"
#include "face_auth_event.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthService::mutex_;
std::shared_ptr<FaceAuthService> FaceAuthService::instance_ = nullptr;
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<FaceAuthService>::GetInstance().get());

std::shared_ptr<FaceAuthService> FaceAuthService::GetInstance()
{
    if (instance_ == nullptr) {
        FACEAUTH_LABEL_LOGE("FaceAuthService GetInstance success!!!!!!!!!!!!!.");
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthService>();
        }
    }
    return instance_;
}

FaceAuthService::FaceAuthService()
    : SystemAbility(SA_ID_FACE_AUTH_SERVICE, true),
      serviceRunningState_(ServiceRunningState::STATE_NOT_START),
      runner_(nullptr)
{}

FaceAuthService::~FaceAuthService()
{}

void FaceAuthService::OnStart()
{
    FACEAUTH_LABEL_LOGI("Start");
    std::string threadName("FaceAuthEventRunner");
    runner_ = AppExecFwk::EventRunner::Create(threadName);
    if (!runner_) {
        FACEAUTH_LABEL_LOGE("failed to create a runner.");
        return;
    }

    runner_->Run();

    if (!Publish(this)) {
        FACEAUTH_LABEL_LOGE("failed to publish the service.");
        return;
    }

    serviceRunningState_ = ServiceRunningState::STATE_RUNNING;

    handler_ = std::make_shared<FaceAuthEventHandler>(runner_);
    if (!handler_) {
        FACEAUTH_LABEL_LOGE("handler_ is nullpter.");
        return;
    }
    FaceAuthManager::GetInstance()->SetEventHandler(handler_);
    FaceAuthEvent::GetInstance()->SetEventHandler(handler_);
    FACEAUTH_LABEL_LOGI("End");
}

void FaceAuthService::OnStop()
{
    serviceRunningState_ = ServiceRunningState::STATE_NOT_START;
}

int32_t FaceAuthService::Init()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_LABEL_LOGI("FaceAuthService Init is called");
    int32_t result = FaceAuthManager::GetInstance()->Init();
    if (result == FI_RC_ERROR) {
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthService::Release()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_LABEL_LOGI("FaceAuthService Release is called");
    return FaceAuthManager::GetInstance()->Release();
}

bool FaceAuthService::HasPermission(std::string permissionName)
{
    return true;
}

int32_t FaceAuthService::Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, challenge: %{private}lld, faceId: %{public}d", param.reqId, param.challenge,
        param.faceId);

    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE("callback is nullptr");
        return FA_RET_ERROR;
    }
    if (HasPermission(Constant::ENROLL_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return FA_RET_ERROR;
    }
    return FaceAuthManager::GetInstance()->Enroll(param, callback);
}

int32_t FaceAuthService::Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, flags: %{public}d, challenge: %{private}lld, "
                        "faceId: %{private}d",
        param.reqId, param.flags, param.challenge, param.faceId);

    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE("callback is nullptr");
        return FA_RET_ERROR;
    }
    if (HasPermission(Constant::ACCESS_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return FA_RET_ERROR;
    }

    return FaceAuthManager::GetInstance()->Authenticate(param, callback);
}

int32_t FaceAuthService::CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu", reqId);
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE("callback is nullptr");
        return FA_RET_ERROR;
    }
    if (HasPermission(Constant::ENROLL_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return FA_RET_ERROR;
    }
    return FaceAuthManager::GetInstance()->CancelEnrollment(reqId, callback);
}

int32_t FaceAuthService::CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu", reqId);
    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE("callback is nullptr");
        return FA_RET_ERROR;
    }
    if (HasPermission(Constant::ACCESS_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return FA_RET_ERROR;
    }
    return FaceAuthManager::GetInstance()->CancelAuth(reqId, callback);
}

int64_t FaceAuthService::PreEnroll()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_LABEL_LOGI("PreEnroll start ");
    if (HasPermission(Constant::ENROLL_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return FA_RET_ERROR;
    }
    int64_t challenge;
    int32_t result = FaceAuthManager::GetInstance()->PreEnroll(challenge, 0);
    FACEAUTH_LABEL_LOGI("FaceAuthService PreEnroll result is %{public}d", result);
    if (result == FA_RET_ERROR) {
        return FA_RET_ERROR;
    }
    return challenge;
}

int32_t FaceAuthService::PostEnroll()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    if (HasPermission(Constant::ENROLL_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return FA_RET_ERROR;
    }
    return FaceAuthManager::GetInstance()->PostEnroll(0);
}

int32_t FaceAuthService::Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId: xxxx%04llu, faceId: %{private}d", param.reqId, param.faceId);

    if (callback == nullptr) {
        FACEAUTH_LABEL_LOGE("callback is nullptr");
        return FA_RET_ERROR;
    }
    if (HasPermission(Constant::ENROLL_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return FA_RET_ERROR;
    }
    return FaceAuthManager::GetInstance()->Remove(param, callback);
}

std::list<int32_t> FaceAuthService::GetEnrolledFaceIDs(const int32_t userId)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    return FaceAuthManager::GetInstance()->GetEnrolledFaceIDs(userId);
}

void FaceAuthService::ResetTimeout()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_LABEL_LOGI("FaceAuthService ResetTimeout is called");
    if (HasPermission(Constant::ACCESS_PERMISSION_NAME) == false) {
        FACEAUTH_LABEL_LOGE("not have faceauth permission");
        return;
    }
    FaceAuthManager::GetInstance()->ResetTimeout();
}

int64_t FaceAuthService::GetRemainingTime()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_LABEL_LOGI("FaceAuthService GetRemainingTime is called");
    return FaceAuthManager::GetInstance()->GetRemainingTime();
}

int32_t FaceAuthService::GetRemainingNum()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_LABEL_LOGI("FaceAuthService GetRemainingNum is called");
    return FaceAuthManager::GetInstance()->GetRemainingNum();
}

int32_t FaceAuthService::GetAngleDim()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    return FaceAuthManager::GetInstance()->GetAngleDim();
}

std::shared_ptr<FaceAuthEventHandler> FaceAuthService::GetEventHandler()
{
    return handler_;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS