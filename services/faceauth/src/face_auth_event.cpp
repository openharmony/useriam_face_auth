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

#include "face_auth_event.h"
#include <future>
#include "face_auth_log_wrapper.h"
#include "face_auth_defines.h"
#include "face_auth_event_handler.h"
#include "face_auth_manager.h"
#include "face_auth_req.h"

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
        FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthEvent::HandleTask event is nullptr");
        return;
    }
    uint32_t event_id = event->GetInnerEventId();
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthEvent::HandleTask inner event id obtained: %{public}u.", event_id);
    FaceOperateType operateType = FaceAuthReq::GetInstance()->GetOperateType(event_id);
    FACEAUTH_HILOGI(MODULE_SERVICE, "operateType is %{public}d", operateType);
    switch (operateType) {
        case FACE_OPERATE_TYPE_LOCAL_AUTH: {
            ProcessAuthenticateTask(event);
            break;
        }
        case FACE_OPERATE_TYPE_ENROLL: {
            ProcessEnrollTask(event);
            break;
        }
        case FACE_OPERATE_TYPE_DEL: {
            ProcessRemoveTask(event);
            break;
        }
        default: {
            FACEAUTH_HILOGE(MODULE_SERVICE, "operateType is invalid, %{public}d", operateType);
            break;
        }
    }
    return;
}
void FaceAuthEvent::ProcessEnrollTask(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<EnrollParam>();
    EnrollParam info = *object;
    uint64_t uId = info.callerUID;
    uint32_t event_id = event->GetInnerEventId();
    bool isCanceled = FaceAuthReq::GetInstance()->isCanceled(event_id, uId);
    if (isCanceled) {
        FaceReqType reqType;
        reqType.reqId = info.scheduleID;
        reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        return;
    }
    FaceAuthManager::GetInstance()->DoEnroll(info);
    return;
}
void FaceAuthEvent::ProcessAuthenticateTask(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<AuthParam>();
    AuthParam info = *object;
    uint64_t uId = info.callerUID;
    uint32_t event_id = event->GetInnerEventId();
    bool isCanceled = FaceAuthReq::GetInstance()->isCanceled(event_id, uId);
    if (isCanceled) {
        FaceReqType reqType;
        reqType.reqId = info.scheduleID;
        reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
        FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
        return;
    }
    FaceAuthManager::GetInstance()->DoAuthenticate(info);
    return;
}
void FaceAuthEvent::ProcessRemoveTask(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto object = event->GetUniqueObject<RemoveParam>();
    RemoveParam info = *object;
    FaceAuthManager::GetInstance()->DoRemove(info);
    return;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
