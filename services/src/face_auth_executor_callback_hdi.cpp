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

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "hdf_base.h"
#include "vibrator_agent.h"

#include "iam_check.h"
#include "iam_executor_iexecute_callback.h"
#include "iam_logger.h"

#include "face_auth_defines.h"
#include "sa_command_manager.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace FaceAuth {

FaceAuthExecutorCallbackHdi::FaceAuthExecutorCallbackHdi(
    std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback, FaceCallbackHdiType faceCallbackHdiType)
    : frameworkCallback_(frameworkCallback), faceCallbackHdiType_(faceCallbackHdiType)
{
}

void FaceAuthExecutorCallbackHdi::DoVibrator()
{
    IAM_LOGI("begin");
    static const char *faceAuthEffect = "haptic.fail";
    bool faceEffectState = false;
    int32_t ret = Sensors::IsSupportEffect(faceAuthEffect, &faceEffectState);
    if (ret != 0) {
        IAM_LOGE("call IsSupportEffect fail %{public}d", ret);
        return;
    }
    if (!faceEffectState) {
        IAM_LOGE("effect not support");
        return;
    }
    if (!Sensors::SetUsage(USAGE_PHYSICAL_FEEDBACK)) {
        IAM_LOGE("call SetUsage fail");
        return;
    }
    ret = Sensors::StartVibrator(faceAuthEffect);
    if (ret != 0) {
        IAM_LOGE("call StartVibrator fail %{public}d", ret);
        return;
    }
    IAM_LOGI("end");
}

int32_t FaceAuthExecutorCallbackHdi::OnResult(int32_t result, const std::vector<uint8_t> &extraInfo)
{
    IAM_LOGI("OnResult %{public}d", result);
    UserAuth::ResultCode retCode = ConvertResultCode(result);
    IF_FALSE_LOGE_AND_RETURN_VAL(frameworkCallback_ != nullptr, HDF_FAILURE);
    if ((faceCallbackHdiType_ == FACE_CALLBACK_AUTH) && (retCode == UserAuth::FAIL)) {
        DoVibrator();
    }
    frameworkCallback_->OnResult(retCode, extraInfo);
    return HDF_SUCCESS;
}

int32_t FaceAuthExecutorCallbackHdi::OnTip(int32_t tip, const std::vector<uint8_t> &extraInfo)
{
    IAM_LOGI("OnTip %{public}d", tip);
    IF_FALSE_LOGE_AND_RETURN_VAL(frameworkCallback_ != nullptr, HDF_FAILURE);
    frameworkCallback_->OnAcquireInfo(tip, extraInfo);
    return HDF_SUCCESS;
}

UserAuth::ResultCode FaceAuthExecutorCallbackHdi::ConvertResultCode(const int32_t in)
{
    ResultCode hdiIn = static_cast<ResultCode>(in);
    if (hdiIn > ResultCode::VENDOR_RESULT_CODE_BEGIN) {
        IAM_LOGI("vendor hdi result code %{public}d, no covert", hdiIn);
        return static_cast<UserAuth::ResultCode>(in);
    }

    static const std::map<ResultCode, UserAuth::ResultCode> data = {
        { ResultCode::SUCCESS, UserAuth::ResultCode::SUCCESS },
        { ResultCode::FAIL, UserAuth::ResultCode::FAIL },
        { ResultCode::GENERAL_ERROR, UserAuth::ResultCode::GENERAL_ERROR },
        { ResultCode::CANCELED, UserAuth::ResultCode::CANCELED },
        { ResultCode::TIMEOUT, UserAuth::ResultCode::TIMEOUT },
        { ResultCode::BUSY, UserAuth::ResultCode::BUSY },
        { ResultCode::INVALID_PARAMETERS, UserAuth::ResultCode::INVALID_PARAMETERS },
        { ResultCode::LOCKED, UserAuth::ResultCode::LOCKED },
        { ResultCode::NOT_ENROLLED, UserAuth::ResultCode::NOT_ENROLLED },
        // should be UserAuth::ResultCode::OPERATION_NOT_SUPPORT
        { ResultCode::OPERATION_NOT_SUPPORT, UserAuth::ResultCode::FAIL },
    };

    UserAuth::ResultCode out;
    auto iter = data.find(hdiIn);
    if (iter == data.end()) {
        out = UserAuth::ResultCode::GENERAL_ERROR;
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
