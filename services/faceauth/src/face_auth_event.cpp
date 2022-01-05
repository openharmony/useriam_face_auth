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

#include "face_auth_event.h"
#include "faceauth_log_wrapper.h"
#include "face_auth_defines.h"
#include "face_auth_event_handler.h"
#include "face_auth_manager.h"
#include "face_auth_req.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthEvent::mutex_;
std::shared_ptr<FaceAuthEvent> FaceAuthEvent::instance_ = nullptr;
FaceAuthEvent::FaceAuthEvent()
{}

FaceAuthEvent::~FaceAuthEvent()
{}

std::shared_ptr<FaceAuthEvent> FaceAuthEvent::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthEvent>();
        }
    }
    return instance_;
}

void FaceAuthEvent::HandleTask(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        FACEAUTH_LABEL_LOGE("FaceAuthEvent::HandleTask event is nullptr");
        return;
    }
    uint32_t event_id = event->GetInnerEventId();
    FACEAUTH_LABEL_LOGI("FaceAuthEvent::HandleTask inner event id obtained: %{public}u.", event_id);
    FaceOperateType operateType = FaceAuthReq::GetInstance()->GetOperateType(event_id);
    FACEAUTH_LABEL_LOGI("operateType is %{public}d", operateType);
    int32_t uId = IPCSkeleton::GetCallingUid();
    bool isCanceled = FaceAuthReq::GetInstance()->isCanceled(event_id, uId);
    switch (operateType) {
        case FACE_OPERATE_TYPE_LOCAL_AUTH: {
            AuthenticateTask(event, isCanceled);
            break;
        }
        case FACE_OPERATE_TYPE_CO_AUTH: {
            CoAuthenticateTask(event, isCanceled);
            break;
        }
        case FACE_OPERATE_TYPE_ENROLL: {
            EnrollTask(event, isCanceled);
            break;
        }
        case FACE_OPERATE_TYPE_DEL: {
            RemoveTask(event);
            break;
        }
        default: {
            FACEAUTH_LABEL_LOGI("operateType is invalid, %{public}d", operateType);
            break;
        }
    }
    return;
}

void FaceAuthEvent::AuthenticateTask(const AppExecFwk::InnerEvent::Pointer &event, const bool isCanceled)
{
    auto object = event->GetUniqueObject<CallsAuthInfo>();
    CallsAuthInfo info = *object;
    if (isCanceled) {
        CallBackParam cbParam;
        cbParam.reqId = info.param.reqId;
        cbParam.code = CODE_CALLBACK_RESULT;
        cbParam.errorCode = ERRCODE_CANCEL;
        FaceAuthManager::GetInstance()->SendCallback(TYPE_CALLBACK_AUTH, cbParam, info.callback);
        FaceReqType reqType;
        reqType.reqId = info.param.reqId;
        reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        return;
    }
    FaceAuthManager::GetInstance()->HandleCallAuthenticate(info.param, info.callback);
    return;
}

void FaceAuthEvent::CoAuthenticateTask(const AppExecFwk::InnerEvent::Pointer &event, const bool isCanceled)
{
    auto object = event->GetUniqueObject<CallsAuthInfo>();
    CallsAuthInfo info = *object;
    if (isCanceled) {
        CallBackParam cbParam;
        cbParam.reqId = info.param.reqId;
        cbParam.code = CODE_CALLBACK_RESULT;
        cbParam.errorCode = ERRCODE_CANCEL;
        FaceAuthManager::GetInstance()->SendCallback(TYPE_CALLBACK_AUTH, cbParam, info.callback);
        FaceReqType reqType;
        reqType.reqId = info.param.reqId;
        reqType.operateType = FACE_OPERATE_TYPE_CO_AUTH;
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        return;
    }
    if (FaceAuthReq::GetInstance()->FindLocalAuth()) {
        FACEAUTH_LABEL_LOGI(
            "There is Local authentication in the current queue. collaborative authentication cannot be performed");
        FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::LOW;
        auto authInfo = std::make_unique<CallsAuthInfo>(info.param, info.callback);
        eventHandler_->SendEvent(event->GetInnerEventId(), std::move(authInfo), priority);
        return;
    }
    FaceAuthManager::GetInstance()->HandleCallAuthenticate(info.param, info.callback);
    return;
}

void FaceAuthEvent::EnrollTask(const AppExecFwk::InnerEvent::Pointer &event, const bool isCanceled)
{
    auto object = event->GetUniqueObject<CallsEnrollInfo>();
    CallsEnrollInfo info = *object;
    if (isCanceled) {
        CallBackParam cbParam;
        cbParam.reqId = info.param.reqId;
        cbParam.code = CODE_CALLBACK_RESULT;
        cbParam.errorCode = ERRCODE_CANCEL;
        FaceAuthManager::GetInstance()->SendCallback(TYPE_CALLBACK_ENROLL, cbParam, info.callback);
        FaceReqType reqType;
        reqType.reqId = info.param.reqId;
        reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        return;
    }
    FaceAuthManager::GetInstance()->HandleCallEnroll(info.param, info.callback);
    return;
}

void FaceAuthEvent::RemoveTask(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<CallsRemoveInfo>();
    CallsRemoveInfo info = *object;
    FaceAuthManager::GetInstance()->HandleCallRemove(info.param, info.callback);
    return;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
