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
            EnrollParam data = {};
            data.scheduleID = scheduleId;
            data.templateID = templateId;
            manager->AddEnrollmentRequest(data);
            break;
        }
        case FACE_COMMAND_AUTH: {
            AuthParam data = {};
            data.scheduleID = scheduleId;
            data.templateID = templateId;
            manager->AddAuthenticationRequest(data);
            break;
        }
        default:
            FACEAUTH_HILOGI(MODULE_SERVICE, "other command.command = %{public}u", command);
            break;
    }
    return FA_RET_OK;
}
int32_t FaceAuthExecutorCallback::OnEndExecute(uint64_t scheduleId, pAuthAttributes consumerAttr)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run start.", __PRETTY_FUNCTION__);
    std::shared_ptr<FaceAuthManager> manager = FaceAuthManager::GetInstance();
    if (manager == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "face auth manager is nullptr.");
        return FA_RET_ERROR;
    }
    // get command
    uint32_t command = 0;
    int32_t ret = FA_RET_OK;
    consumerAttr->GetUint32Value(AUTH_SCHEDULE_MODE, command);
    FACEAUTH_HILOGI(MODULE_SERVICE, "command = %{public}u.", command);
    switch (command) {
        case FACE_COMMAND_CANCEL_ENROLL: {
            EnrollParam data = {};
            data.scheduleID = scheduleId;
            ret = manager->CancelEnrollment(data);
            if (ret != FA_RET_OK) {
                return FA_RET_GENERAL_ERROR;
            }
            break;
        }
        case FACE_COMMAND_CANCEL_AUTH: {
            AuthParam data = {};
            data.scheduleID = scheduleId;
            ret = manager->CancelAuth(data);
            if (ret != FA_RET_OK) {
                return FA_RET_GENERAL_ERROR;
            }
            break;
        }
        default:
            FACEAUTH_HILOGI(MODULE_SERVICE, "other command.command = %{public}u", command);
            break;
    }
    return ret;
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
    std::vector<uint64_t> templateIdList;
    properties->GetUint64ArrayValue(AUTH_TEMPLATE_ID_LIST, templateIdList);
    std::string bundleName = "";
    bundleName.assign(callerName.begin(), callerName.end());
    switch (command) {
        case FACE_COMMAND_REMOVE: {
            RemoveParam data = {};
            data.scheduleID = scheduleID;
            data.templateID = templateID;
            manager->AddRemoveRequest(data);
            break;
        }
        case FACE_COMMAND_PROPERMODE_FREEZE:
            manager->FreezeTemplates(templateIdList);
            break;
        case FACE_COMMAND_PROPERMODE_UNFREEZE:
            manager->UnfreezeTemplates(templateIdList);
            break;
        case FACE_COMMAND_INIT_ALGORITHM:
            manager->InitAlgorithm(bundleName);
            break;
        case FACE_COMMAND_RELEASE_ALGORITHM:
            manager->ReleaseAlgorithm(bundleName);
            break;
        default:
            FACEAUTH_HILOGI(MODULE_SERVICE, "other command.command = %{public}u", command);
            break;
    }
    return FA_RET_OK;
}
int32_t FaceAuthExecutorCallback::OnGetProperty(std::shared_ptr<AuthResPool::AuthAttributes> conditions,
    std::shared_ptr<AuthResPool::AuthAttributes> values)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthService::OnGetProperty enter");
    if (values == nullptr || conditions == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthService::OnGetProperty bad param");
        return FA_RET_ERROR;
    }
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA is nullptr.");
        return FA_RET_ERROR;
    }

    /* set command 0:delete 1:Query credential information */
    uint32_t command;
    if (conditions->GetUint32Value(AUTH_PROPERTY_MODE, command) != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthService::OnGetProperty GetUint32Value");
        return FA_RET_ERROR;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE,
        "FaceAuthService::OnBeginExecute AUTH_PROPERTY_MODE is %{public}u.", command);
    if (command == FACE_COMMAND_QUERY_CREDENTIAL) {
        /* get templateId */
        uint64_t templateId;
        if (conditions->GetUint64Value(AUTH_TEMPLATE_ID, templateId) != FA_RET_OK) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthService::OnGetProperty GetUint64Value");
            return FA_RET_ERROR;
        }
        /* Query credential information */
        FaceCredentialInfo info;
        faceAuthCA->GetFaceInfo(templateId, info);
        if (values->SetUint64Value(AUTH_SUBTYPE, info.subType) != FA_RET_OK) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthService::OnGetProperty SetUint64Value");
            return FA_RET_ERROR;
        }
        /* send remainTimes FreezingTime */
        if (values->SetUint32Value(AUTH_REMAIN_TIME, info.freezingTime)) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthService::OnGetProperty SetUint32ArrayValue");
            return FA_RET_ERROR;
        }
        if (values->SetUint32Value(AUTH_REMAIN_COUNT, info.remainTimes)) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthService::OnGetProperty SetUint32ArrayValue");
            return FA_RET_ERROR;
        }
    }
    return FA_RET_OK;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
