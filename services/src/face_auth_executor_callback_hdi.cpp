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

#include <functional>
#include <cstdint>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "hdf_base.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iexecute_callback.h"

#include "face_auth_defines.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
FaceAuthExecutorCallbackHdi::FaceAuthExecutorCallbackHdi(std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback)
    : frameworkCallback_(frameworkCallback)
{
}

int32_t FaceAuthExecutorCallbackHdi::OnResult(int32_t result, const std::vector<uint8_t> &extraInfo)
{
    IAM_LOGI("OnResult %{public}d", result);
    UserIam::UserAuth::ResultCode retCode = ConvertResultCode(result);
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

UserIam::UserAuth::ResultCode FaceAuthExecutorCallbackHdi::ConvertResultCode(const int32_t in)
{
    ResultCode hdiIn = static_cast<ResultCode>(in);
    if (hdiIn > ResultCode::VENDOR_RESULT_CODE_BEGIN) {
        IAM_LOGI("vendor hdi result code %{public}d, no covert", hdiIn);
        return static_cast<UserIam::UserAuth::ResultCode>(in);
    }

    static const std::map<ResultCode, UserIam::UserAuth::ResultCode> data = {
        {ResultCode::SUCCESS, UserIam::UserAuth::ResultCode::SUCCESS},
        {ResultCode::FAIL, UserIam::UserAuth::ResultCode::FAIL},
        {ResultCode::GENERAL_ERROR, UserIam::UserAuth::ResultCode::GENERAL_ERROR},
        {ResultCode::CANCELED, UserIam::UserAuth::ResultCode::CANCELED},
        {ResultCode::TIMEOUT, UserIam::UserAuth::ResultCode::TIMEOUT},
        {ResultCode::BUSY, UserIam::UserAuth::ResultCode::BUSY},
        {ResultCode::INVALID_PARAMETERS, UserIam::UserAuth::ResultCode::INVALID_PARAMETERS},
        {ResultCode::LOCKED, UserIam::UserAuth::ResultCode::LOCKED},
        {ResultCode::NOT_ENROLLED, UserIam::UserAuth::ResultCode::NOT_ENROLLED},
        // should be UserIam::UserAuth::ResultCode::OPERATION_NOT_SUPPORT
        {ResultCode::OPERATION_NOT_SUPPORT, UserIam::UserAuth::ResultCode::FAIL},
    };

    UserIam::UserAuth::ResultCode out;
    auto iter = data.find(hdiIn);
    if (iter == data.end()) {
        out = UserIam::UserAuth::ResultCode::GENERAL_ERROR;
        IAM_LOGE("convert hdi undefined result code %{public}d to framework result code %{public}d", in, out);
        return out;
    }
    out = iter->second;
    IAM_LOGI("covert hdi result code %{public}d to framework result code %{public}d", hdiIn, out);
    return out;
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
