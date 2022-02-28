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
#include "securec.h"
#include "face_auth_manager.h"
#include "face_auth_log_wrapper.h"
#include "coauth_info_define.h"
#include "face_auth_ca.h"
#include "face_auth_executor_callback.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
int32_t FaceAuthExecutorCallback::OnBeginExecute(uint64_t scheduleId, std::vector<uint8_t> &publicKey,
                                                 pAuthAttributes commandAttrs)
{
    (void)(publicKey);
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    std::shared_ptr<FaceAuthManager> manager = FaceAuthManager::GetInstance();
    if (manager == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "face auth manager is nullptr.");
        return FA_RET_ERROR;
    }
    // get command
    uint32_t command = 0;
    commandAttrs->GetUint32Value(AUTH_SCHEDULE_MODE, command);
    FACEAUTH_HILOGI(MODULE_SERVICE, "command = %{public}u.", command);
    // get templateID
    uint64_t templateId = 0;
    commandAttrs->GetUint64Value(AUTH_TEMPLATE_ID, templateId);
    switch (command) {
        case FACE_COMMAND_ENROLL: {
            EnrollParam data;
            memset_s(&data, sizeof(EnrollParam), 0, sizeof(EnrollParam));
            data.scheduleID = scheduleId;
            data.templateID = templateId;
            manager->Enrollment(data);
            break;
        }
        case FACE_COMMAND_AUTH: {
            AuthParam data;
            memset_s(&data, sizeof(AuthParam), 0, sizeof(AuthParam));
            data.scheduleID = scheduleId;
            data.templateID = templateId;
            manager->Authenticate(data);
            break;
        }
        case FACE_COMMAND_CANCEL_ENROLL: {
            EnrollParam data;
            memset_s(&data, sizeof(EnrollParam), 0, sizeof(EnrollParam));
            data.scheduleID = scheduleId;
            manager->CancelEnrollment(data);
            break;
        }
        case FACE_COMMAND_CANCEL_AUTH: {
            AuthParam data;
            memset_s(&data, sizeof(AuthParam), 0, sizeof(AuthParam));
            data.scheduleID = scheduleId;
            manager->CancelAuth(data);
            break;
        }
        default:
            FACEAUTH_HILOGI(MODULE_SERVICE, "other command.command = %u", command);
            break;
    }
    return FA_RET_OK;
}
int32_t FaceAuthExecutorCallback::OnEndExecute(uint64_t scheduleId, pAuthAttributes consumerAttr)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    (void)(scheduleId);
    (void)(consumerAttr);
    return FA_RET_OK;
}

void FaceAuthExecutorCallback::OnMessengerReady(const sptr<AuthResPool::IExecutorMessenger> &messenger)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    std::shared_ptr<FaceAuthManager> manager = FaceAuthManager::GetInstance();
    if (manager == nullptr) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "manager instance is null.");
        return;
    }
    manager->SetExecutorMessenger(messenger);
    manager->VerifyAuthInfo();
}

int32_t FaceAuthExecutorCallback::OnSetProperty(pAuthAttributes properties)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    std::shared_ptr<FaceAuthManager> manager = FaceAuthManager::GetInstance();
    if (manager == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "face auth manager is nullptr.");
        return FA_RET_ERROR;
    }
    // get command
    uint32_t command = 0;
    properties->GetUint32Value(AUTH_PROPERTY_MODE, command);
    FACEAUTH_HILOGI(MODULE_SERVICE, "command = %{public}u.", command);
    // get scheduleID
    uint64_t scheduleID = 0;
    properties->GetUint64Value(AUTH_SESSION_ID, scheduleID);
    // get templateID
    uint64_t templateID = 0;
    properties->GetUint64Value(AUTH_TEMPLATE_ID, templateID);
    // get caller name
    std::vector<uint8_t> callerName;
    properties->GetUint8ArrayValue(ALGORITHM_INFO, callerName);
    std::string bundleName = "";
    bundleName.assign(callerName.begin(), callerName.end());
    switch (command) {
        case FACE_COMMAND_REMOVE: {
            RemoveParam data;
            memset_s(&data, sizeof(RemoveParam), 0, sizeof(RemoveParam));
            data.scheduleID = scheduleID;
            data.templateID = templateID;
            manager->Remove(data);
            break;
        }
        case FACE_COMMAND_INIT_ALGORITHM:
            manager->InitAlgorithm(bundleName);
            break;
        case FACE_COMMAND_RELEASE_ALGORITHM:
            manager->ReleaseAlgorithm(bundleName);
            break;
        default:
            FACEAUTH_HILOGI(MODULE_SERVICE, "other command.command = %u", command);
            break;
    }
    return FA_RET_OK;
}
int32_t FaceAuthExecutorCallback::OnGetProperty(std::shared_ptr<AuthResPool::AuthAttributes> conditions,
                                                std::shared_ptr<AuthResPool::AuthAttributes> values)
{
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA is nullptr.");
        return FA_RET_ERROR;
    }
    uint64_t templateID = 0;
    int32_t remainingTimes = 0;
    conditions->GetUint64Value(AUTH_TEMPLATE_ID, templateID);
    faceAuthCA->GetRemainTimes(templateID, remainingTimes);
    values->SetUint64Value(AUTH_REMAIN_TIME, remainingTimes);

    return FA_RET_OK;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
