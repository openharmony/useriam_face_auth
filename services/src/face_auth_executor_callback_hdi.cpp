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

#include "face_auth_executor_callback_hdi.h"

#include "face_auth_defines.h"
#include "iam_check.h"
#include "iam_logger.h"
#include "v1_0/face_auth_types.h"

#define LOG_LABEL UserIAM::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthExecutorCallbackHdi::FaceAuthExecutorCallbackHdi(std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback)
    : frameworkCallback_(frameworkCallback)
{
}

int32_t FaceAuthExecutorCallbackHdi::OnResult(int32_t result, const std::vector<uint8_t> &extraInfo)
{
    IAM_LOGI("OnResult %{public}d", result);
    UserIAM::ResultCode retCode = ConvertResultCode(result);
    IF_FALSE_LOGE_AND_RETURN_VAL(frameworkCallback_ != nullptr, HDF_FAILURE);
    frameworkCallback_->OnResult(retCode, extraInfo);
    return HDF_SUCCESS;
}

int32_t FaceAuthExecutorCallbackHdi::OnAcquireInfo(int32_t acquire, const std::vector<uint8_t> &extraInfo)
{
    IAM_LOGI("OnAcquireInfo %{public}d", acquire);
    IF_FALSE_LOGE_AND_RETURN_VAL(frameworkCallback_ != nullptr, HDF_FAILURE);
    frameworkCallback_->OnAcquireInfo(acquire, extraInfo);
    return HDF_SUCCESS;
}

UserIAM::ResultCode FaceAuthExecutorCallbackHdi::ConvertResultCode(const int32_t in)
{
    ResultCode hdiIn = static_cast<ResultCode>(in);
    if (hdiIn > ResultCode::VENDOR_RESULT_CODE_BEGIN) {
        IAM_LOGI("vendor hdi result code %{public}d, no covert", hdiIn);
        return static_cast<UserIAM::ResultCode>(in);
    }

    static const std::map<ResultCode, UserIAM::ResultCode> data = {
        {ResultCode::SUCCESS, UserIAM::ResultCode::SUCCESS},
        {ResultCode::FAIL, UserIAM::ResultCode::FAIL},
        {ResultCode::GENERAL_ERROR, UserIAM::ResultCode::GENERAL_ERROR},
        {ResultCode::CANCELED, UserIAM::ResultCode::CANCELED},
        {ResultCode::TIMEOUT, UserIAM::ResultCode::TIMEOUT},
        {ResultCode::BUSY, UserIAM::ResultCode::BUSY},
        {ResultCode::INVALID_PARAMETERS, UserIAM::ResultCode::INVALID_PARAMETERS},
        {ResultCode::LOCKED, UserIAM::ResultCode::LOCKED},
        {ResultCode::NOT_ENROLLED, UserIAM::ResultCode::NOT_ENROLLED},
        // should be UserIAM::ResultCode::OPERATION_NOT_SUPPORT
        {ResultCode::OPERATION_NOT_SUPPORT, UserIAM::ResultCode::FAIL},
    };

    UserIAM::ResultCode out;
    auto iter = data.find(hdiIn);
    if (iter == data.end()) {
        out = UserIAM::ResultCode::GENERAL_ERROR;
        IAM_LOGE("convert hdi undefined result code %{public}d to framework result code %{public}d", in, out);
        return out;
    }
    out = iter->second;
    IAM_LOGI("covert hdi result code %{public}d to framework result code %{public}d", hdiIn, out);
    return out;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
