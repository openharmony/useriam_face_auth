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

#include "hdf_base.h"

#include "face_auth_defines.h"
#include "face_auth_executor_callback_hdi.h"
#include "iam_check.h"
#include "iam_logger.h"

#define LOG_LABEL UserIAM::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
using IamResultCode = OHOS::UserIam::UserAuth::ResultCode;
using IamExecutorRole = UserIam::UserAuth::ExecutorRole;
using IamExecutorInfo = UserIam::UserAuth::ExecutorInfo;
FaceAuthExecutorHdi::FaceAuthExecutorHdi(sptr<FaceHdi::IExecutor> executorProxy) : executorProxy_(executorProxy) {};

IamResultCode FaceAuthExecutorHdi::GetExecutorInfo(IamExecutorInfo &info)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    FaceHdi::ExecutorInfo localInfo = {};
    int32_t status = executorProxy_->GetExecutorInfo(localInfo);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("GetExecutorInfo fail result %{public}d", result);
        return result;
    }
    result = MoveHdiExecutorInfo(localInfo, info);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("MoveHdiExecutorInfo fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::GetTemplateInfo(uint64_t templateId, UserAuth::TemplateInfo &info)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    FaceHdi::TemplateInfo localInfo = {};
    int32_t status = executorProxy_->GetTemplateInfo(templateId, localInfo);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("GetTemplateInfo fail result %{public}d", result);
        return result;
    }
    MoveHdiTemplateInfo(localInfo, info);
    
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
    const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->OnRegisterFinish(templateIdList, frameworkPublicKey, extraInfo);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("OnRegisterFinish fail result %{public}d", status);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::Enroll(uint64_t scheduleId, uint32_t tokenId,
    const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    auto callback = sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Enroll(scheduleId, extraInfo, callback);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Enroll fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::Authenticate(uint64_t scheduleId, uint32_t tokenId,
    const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &extraInfo,
    const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    auto callback = sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Authenticate(scheduleId, templateIdList, extraInfo, callback);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Authenticate fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::Identify(uint64_t scheduleId, uint32_t tokenId,
    const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    auto callback = sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Identify(scheduleId, extraInfo, callback);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Identify fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::Delete(const std::vector<uint64_t> &templateIdList)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Delete(templateIdList);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Delete fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::Cancel(uint64_t scheduleId)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Cancel(scheduleId);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Cancel fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::SendCommand(UserIam::UserAuth::PropertyMode commandId,
    const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    FaceHdi::CommandId hdiCommandId;
    IamResultCode result = ConvertCommandId(commandId, hdiCommandId);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertCommandId fail result %{public}d", result);
        return result;
    }
    auto callback = sptr<FaceHdi::IExecutorCallback>(new (std::nothrow) FaceAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->SendCommand(hdiCommandId, extraInfo, callback);
    result = ConvertResultCode(status);
    if (status != IamResultCode::SUCCESS) {
        IAM_LOGE("SendCommand fail result %{public}d", status);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::MoveHdiExecutorInfo(FaceHdi::ExecutorInfo &in, IamExecutorInfo &out)
{
    out.executorSensorHint = static_cast<int32_t>(in.sensorId);
    out.executorMatcher = static_cast<int32_t>(in.executorType);
    IamResultCode result = ConvertExecutorRole(in.executorRole, out.executorRole);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertExecutorRole fail result %{public}d", result);
        return result;
    }
    result = ConvertAuthType(in.authType, out.authType);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertAuthType fail result %{public}d", result);
        return result;
    }
    result = ConvertExecutorSecureLevel(in.esl, out.esl);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertExecutorSecureLevel fail result %{public}d", result);
        return result;
    }
    in.publicKey.swap(out.publicKey);
    return IamResultCode::SUCCESS;
}

void FaceAuthExecutorHdi::MoveHdiTemplateInfo(FaceHdi::TemplateInfo &in, UserAuth::TemplateInfo &out)
{
    out.executorType = in.executorType;
    out.freezingTime = in.freezingTime;
    out.remainTimes = in.remainTimes;
    in.extraInfo.swap(out.extraInfo);
}

IamResultCode FaceAuthExecutorHdi::ConvertCommandId(const UserIam::UserAuth::PropertyMode in, FaceHdi::CommandId &out)
{
    if (static_cast<FaceHdi::CommandId>(in) > FaceHdi::CommandId::VENDOR_COMMAND_BEGIN) {
        out = static_cast<FaceHdi::CommandId>(in);
        IAM_LOGI("vendor command id %{public}d, no covert", out);
        return IamResultCode::SUCCESS;
    }

    static const std::map<UserIam::UserAuth::PropertyMode, FaceHdi::CommandId> data = {
        {UserIam::UserAuth::PropertyMode::PROPERTY_MODE_FREEZE, FaceHdi::CommandId::LOCK_TEMPLATE},
        {UserIam::UserAuth::PropertyMode::PROPERTY_MODE_UNFREEZE, FaceHdi::CommandId::UNLOCK_TEMPLATE}};
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("command id %{public}d is invalid", in);
        return IamResultCode::INVALID_PARAMETERS;
    }
    out = iter->second;
    IAM_LOGI("covert command id %{public}d to idl command is %{public}d", in, out);
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::ConvertAuthType(const FaceHdi::AuthType in, UserIam::UserAuth::AuthType &out)
{
    static const std::map<FaceHdi::AuthType, UserIam::UserAuth::AuthType> data = {
        {FaceHdi::FACE, UserIam::UserAuth::AuthType::FACE},
    };
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("authType %{public}d is invalid", in);
        return IamResultCode::GENERAL_ERROR;
    }
    out = iter->second;
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::ConvertExecutorRole(const FaceHdi::ExecutorRole in, IamExecutorRole &out)
{
    static const std::map<FaceHdi::ExecutorRole, IamExecutorRole> data = {
        {FaceHdi::ExecutorRole::COLLECTOR, IamExecutorRole::COLLECTOR},
        {FaceHdi::ExecutorRole::VERIFIER, IamExecutorRole::VERIFIER},
        {FaceHdi::ExecutorRole::ALL_IN_ONE, IamExecutorRole::ALL_IN_ONE},
    };
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("executorRole %{public}d is invalid", in);
        return IamResultCode::GENERAL_ERROR;
    }
    out = iter->second;
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::ConvertExecutorSecureLevel(
    const FaceHdi::ExecutorSecureLevel in, UserIam::UserAuth::ExecutorSecureLevel &out)
{
    static const std::map<FaceHdi::ExecutorSecureLevel, UserIam::UserAuth::ExecutorSecureLevel> data = {
        {FaceHdi::ExecutorSecureLevel::ESL0, UserIam::UserAuth::ExecutorSecureLevel::ESL0},
        {FaceHdi::ExecutorSecureLevel::ESL1, UserIam::UserAuth::ExecutorSecureLevel::ESL1},
        {FaceHdi::ExecutorSecureLevel::ESL2, UserIam::UserAuth::ExecutorSecureLevel::ESL2},
        {FaceHdi::ExecutorSecureLevel::ESL3, UserIam::UserAuth::ExecutorSecureLevel::ESL3},
    };
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("executorSecureLevel %{public}d is invalid", in);
        return IamResultCode::GENERAL_ERROR;
    }
    out = iter->second;
    return IamResultCode::SUCCESS;
}

IamResultCode FaceAuthExecutorHdi::ConvertResultCode(const int32_t in)
{
    HDF_STATUS hdfIn = static_cast<HDF_STATUS>(in);
    static const std::map<HDF_STATUS, IamResultCode> data = {
        {HDF_SUCCESS, IamResultCode::SUCCESS},
        {HDF_FAILURE, IamResultCode::GENERAL_ERROR},
        {HDF_ERR_TIMEOUT, IamResultCode::TIMEOUT},
        {HDF_ERR_QUEUE_FULL, IamResultCode::BUSY},
        {HDF_ERR_DEVICE_BUSY, IamResultCode::BUSY},
    };

    IamResultCode out;
    auto iter = data.find(hdfIn);
    if (iter == data.end()) {
        out = IamResultCode::GENERAL_ERROR;
    } else {
        out = iter->second;
    }
    IAM_LOGI("covert hdi result code %{public}d to framework result code %{public}d", in, out);
    return out;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
