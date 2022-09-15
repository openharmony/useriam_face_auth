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

#include "face_auth_driver_hdi.h"

#include <memory>
#include <vector>

#include "hdf_base.h"
#include "refbase.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "face_auth_defines.h"
#include "face_auth_executor_hdi.h"
#include "face_auth_interface_adapter.h"
#include "v1_0/face_auth_types.h"
#include "v1_0/iexecutor.h"
#include "v1_0/iface_auth_interface.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA
using namespace OHOS::HDI::FaceAuth::V1_0;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
using namespace OHOS::UserIam;

std::mutex FaceAuthDriverHdi::mutex_;

FaceAuthDriverHdi::FaceAuthDriverHdi(const std::shared_ptr<FaceAuthInterfaceAdapter> faceAuthInterfaceAdapter)
    : faceAuthInterfaceAdapter_(faceAuthInterfaceAdapter)
{
}

void FaceAuthDriverHdi::GetExecutorList(std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> &executorList)
{
    IF_FALSE_LOGE_AND_RETURN(faceAuthInterfaceAdapter_ != nullptr);
    auto faceIf = faceAuthInterfaceAdapter_->Get();
    if (faceIf == nullptr) {
        IAM_LOGE("IFaceAuthInterface is null");
        return;
    }

    std::vector<sptr<IExecutor>> iExecutorList;
    auto ret = faceIf->GetExecutorList(iExecutorList);
    if (ret != HDF_SUCCESS) {
        IAM_LOGE("GetExecutorList fail");
        return;
    }

    std::lock_guard<std::mutex> gurard(mutex_);
    faceAuthExecutorList_.clear();
    for (const auto &iExecutor : iExecutorList) {
        if (iExecutor == nullptr) {
            IAM_LOGE("iExecutor is nullptr");
            continue;
        }
        auto executor = Common::MakeShared<FaceAuthExecutorHdi>(iExecutor);
        if (executor == nullptr) {
            IAM_LOGE("make share failed");
            continue;
        }
        executorList.push_back(executor);
        faceAuthExecutorList_.push_back(executor);
    }
}

int32_t FaceAuthDriverHdi::SetBufferProducer(sptr<IBufferProducer> &producer)
{
    if (faceAuthExecutorList_.size() == 0) {
        IAM_LOGE("no executor to set buffer producer");
        return FACEAUTH_ERROR;
    }

    std::lock_guard<std::mutex> gurard(mutex_);
    for (auto executor : faceAuthExecutorList_) {
        IF_FALSE_LOGE_AND_RETURN_VAL(executor != nullptr, FACEAUTH_ERROR);
        int32_t ret = executor->SetBufferProducer(producer);
        if (ret != FACEAUTH_SUCCESS) {
            IAM_LOGE("executor SetBufferProducer fail %{public}d", ret);
            return FACEAUTH_ERROR;
        }
    }
    return FACEAUTH_SUCCESS;
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
