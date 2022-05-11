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

#include "face_auth_executor_hdi.h"

#include "face_auth_defines.h"
#include "face_auth_executor_callback_hdi.h"
#include "hdf_base.h"
#include "iam_check.h"
#include "iam_logger.h"

#define LOG_LABEL UserIAM::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthExecutorHdi::FaceAuthExecutorHdi(sptr<FaceHdi::IExecutor> executorProxy) : executorProxy_(executorProxy) {};

UserIAM::ResultCode FaceAuthExecutorHdi::GetExecutorInfo(UserIAM::ExecutorInfo &info)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    FaceHdi::ExecutorInfo localInfo = {};
    int32_t status = executorProxy_->GetExecutorInfo(localInfo);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("GetExecutorInfo fail ret=%{public}d", result);
        return result;
    }
    int32_t ret = MoveHdiExecutorInfo(localInfo, info);
    if (ret != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("MoveHdiExecutorInfo fail ret=%{public}d", ret);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::GetTemplateInfo(uint64_t templateId, UserAuth::TemplateInfo &info)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    FaceHdi::TemplateInfo localInfo = {};
    int32_t status = executorProxy_->GetTemplateInfo(templateId, localInfo);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("GetTemplateInfo fail ret=%{public}d", result);
        return result;
    }
    int32_t ret = MoveHdiTemplateInfo(localInfo, info);
    if (ret != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("MoveHdiTemplateInfo fail ret=%{public}d", ret);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
    const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->OnRegisterFinish(templateIdList, frameworkPublicKey, extraInfo);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("OnRegisterFinish fail ret=%{public}d", status);
        return result;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::Enroll(uint64_t scheduleId, uint64_t callerUid,
    const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    auto callback = sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Enroll(scheduleId, extraInfo, callback);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("Enroll fail ret=%{public}d", result);
        return result;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::Authenticate(uint64_t scheduleId, uint64_t callerUid,
    const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &extraInfo,
    const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    auto callback = sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Authenticate(scheduleId, templateIdList, extraInfo, callback);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("Authenticate fail ret=%{public}d", result);
        return result;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::Identify(uint64_t scheduleId, uint64_t callerUid,
    const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    auto callback = sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Identify(scheduleId, extraInfo, callback);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("Identify fail ret=%{public}d", result);
        return result;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::Delete(const std::vector<uint64_t> &templateIdList)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Delete(templateIdList);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("Delete fail ret=%{public}d", result);
        return result;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::Cancel(uint64_t scheduleId)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Cancel(scheduleId);
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (result != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("Cancel fail ret=%{public}d", result);
        return result;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::SendCommand(UserAuth::AuthPropertyMode commandId,
    const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, UserIAM::ResultCode::GENERAL_ERROR);
    FaceHdi::CommandId hdiCommandId;
    int32_t ret = ConvertCommandId(commandId, hdiCommandId);
    if (ret != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("ConvertCommandId fail ret=%{public}d", ret);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    int32_t status = executorProxy_->SendCommand(hdiCommandId, extraInfo,
        sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj)));
    UserIAM::ResultCode result = ConvertResultCode(status);
    if (status != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("SendCommand fail ret=%{public}d", status);
        return result;
    }
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::MoveHdiExecutorInfo(FaceHdi::ExecutorInfo &in, UserIAM::ExecutorInfo &out)
{
    out.executorId = static_cast<int32_t>(in.sensorId);
    out.executorType = static_cast<int32_t>(in.executorType);
    int32_t ret = ConvertExecutorRole(in.executorRole, out.role);
    if (ret != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("executorProxy is null");
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    ret = ConvertAuthType(in.authType, out.authType);
    if (ret != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("ConvertAuthType fail ret=%{public}d", ret);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    ret = ConvertExecutorSecureLevel(in.esl, out.esl);
    if (ret != UserIAM::ResultCode::SUCCESS) {
        IAM_LOGE("ConvertExecutorSecureLevel fail ret=%{public}d", ret);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    in.publicKey.swap(out.publicKey);
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::MoveHdiTemplateInfo(FaceHdi::TemplateInfo &in, UserAuth::TemplateInfo &out)
{
    out.executorType = in.executorType;
    out.freezingTime = in.freezingTime;
    out.remainTimes = in.remainTimes;
    in.extraInfo.swap(out.extraInfo);
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::ConvertCommandId(const UserAuth::AuthPropertyMode in, FaceHdi::CommandId &out)
{
    if (static_cast<UserAuth::CommandId>(in) > UserAuth::VENDOR_COMMAND_BEGIN) {
        out = static_cast<FaceHdi::CommandId>(in);
        IAM_LOGI("vendor command id %{public}d, no covert", out);
        return UserIAM::ResultCode::SUCCESS;
    }

    static const std::map<UserAuth::AuthPropertyMode, FaceHdi::CommandId> data = {
        {UserAuth::AuthPropertyMode::PROPERMODE_FREEZE, FaceHdi::CommandId::LOCK_TEMPLATE},
        {UserAuth::AuthPropertyMode::PROPERMODE_UNFREEZE, FaceHdi::CommandId::UNLOCK_TEMPLATE}};
    if (data.count(in) == 0) {
        IAM_LOGE("command id %{public}d is invalid", in);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    out = data.at(in);
    IAM_LOGI("covert command id %{public}d to idl command is %{public}d", in, out);
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::ConvertAuthType(const FaceHdi::AuthType in, UserIAM::AuthType &out)
{
    static const std::map<FaceHdi::AuthType, UserIAM::AuthType> data = {
        {FaceHdi::FACE, UserIAM::AuthType::FACE},
    };
    if (data.count(in) == 0) {
        IAM_LOGE("authType %{public}d is invalid", in);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    out = data.at(in);
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::ConvertExecutorRole(const FaceHdi::ExecutorRole in, UserIAM::ExecutorRole &out)
{
    static const std::map<FaceHdi::ExecutorRole, UserIAM::ExecutorRole> data = {
        {FaceHdi::ExecutorRole::COLLECTOR, UserIAM::ExecutorRole::COLLECTOR},
        {FaceHdi::ExecutorRole::VERIFIER, UserIAM::ExecutorRole::VERIFIER},
        {FaceHdi::ExecutorRole::ALL_IN_ONE, UserIAM::ExecutorRole::ALL_IN_ONE},
    };
    if (data.count(in) == 0) {
        IAM_LOGE("executorRole %{public}d is invalid", in);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    out = data.at(in);
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::ConvertExecutorSecureLevel(
    const FaceHdi::ExecutorSecureLevel in, UserIAM::ExecutorSecureLevel &out)
{
    static const std::map<FaceHdi::ExecutorSecureLevel, UserIAM::ExecutorSecureLevel> data = {
        {FaceHdi::ExecutorSecureLevel::ESL0, UserIAM::ExecutorSecureLevel::ESL0},
        {FaceHdi::ExecutorSecureLevel::ESL1, UserIAM::ExecutorSecureLevel::ESL1},
        {FaceHdi::ExecutorSecureLevel::ESL2, UserIAM::ExecutorSecureLevel::ESL2},
        {FaceHdi::ExecutorSecureLevel::ESL3, UserIAM::ExecutorSecureLevel::ESL3},
    };
    if (data.count(in) == 0) {
        IAM_LOGE("executorSecureLevel %{public}d is invalid", in);
        return UserIAM::ResultCode::GENERAL_ERROR;
    }
    out = data.at(in);
    return UserIAM::ResultCode::SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorHdi::ConvertResultCode(const int32_t in)
{
    HDF_STATUS hdfIn = static_cast<HDF_STATUS>(in);
    static const std::map<HDF_STATUS, UserIAM::ResultCode> data = {
        {HDF_SUCCESS, UserIAM::ResultCode::SUCCESS},
        {HDF_FAILURE, UserIAM::ResultCode::FAIL},
        {HDF_ERR_TIMEOUT, UserIAM::ResultCode::TIMEOUT},
        {HDF_ERR_QUEUE_FULL, UserIAM::ResultCode::BUSY},
        {HDF_ERR_DEVICE_BUSY, UserIAM::ResultCode::BUSY},
    };

    UserIAM::ResultCode out;
    if (data.count(hdfIn) == 0) {
        out = UserIAM::ResultCode::GENERAL_ERROR;
    } else {
        out = data.at(hdfIn);
    }
    IAM_LOGE("covert hdi result code %{public}d to framework result code %{public}d", in, out);
    return out;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
