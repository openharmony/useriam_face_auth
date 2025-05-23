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

#include "face_auth_all_in_one_executor_hdi.h"
#include "face_auth_defines.h"
#include "face_auth_hdi.h"
#include "face_auth_interface_adapter.h"

#define LOG_TAG "FACE_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
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

    std::vector<sptr<IAllInOneExecutor>> iExecutorList;
    auto ret = faceIf->GetExecutorList(iExecutorList);
    if (ret != HDF_SUCCESS) {
        IAM_LOGE("GetExecutorList fail");
        return;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    faceAuthExecutorList_.clear();
    for (const auto &iExecutor : iExecutorList) {
        if (iExecutor == nullptr) {
            IAM_LOGE("iExecutor is nullptr");
            continue;
        }
        auto executor = Common::MakeShared<FaceAuthAllInOneExecutorHdi>(iExecutor);
        if (executor == nullptr) {
            IAM_LOGE("make share failed");
            continue;
        }
        executorList.push_back(executor);
        faceAuthExecutorList_.push_back(executor);
    }
}

void FaceAuthDriverHdi::OnHdiDisconnect()
{
    IAM_LOGI("start");
    std::lock_guard<std::mutex> guard(mutex_);
    for (const auto &iExecutor : faceAuthExecutorList_) {
        if (iExecutor == nullptr) {
            IAM_LOGE("iExecutor is nullptr");
            continue;
        }
        iExecutor->OnHdiDisconnect();
    }
    faceAuthExecutorList_.clear();
    return;
}

int32_t FaceAuthDriverHdi::SetBufferProducer(sptr<IBufferProducer> &producer)
{
    OHOS::sptr<BufferProducerSequenceable> producerSequenceable(nullptr);
    if (producer != nullptr) {
        producerSequenceable =
            sptr<BufferProducerSequenceable>(new (std::nothrow) BufferProducerSequenceable(producer));
        IF_FALSE_LOGE_AND_RETURN_VAL(producerSequenceable != nullptr, FACE_AUTH_ERROR);
    }

    IF_FALSE_LOGE_AND_RETURN_VAL(faceAuthInterfaceAdapter_ != nullptr, FACE_AUTH_ERROR);
    auto faceIf = faceAuthInterfaceAdapter_->Get();
    if (faceIf == nullptr) {
        IAM_LOGE("IFaceAuthInterface is null");
        return FACE_AUTH_ERROR;
    }

    auto ret = faceIf->SetBufferProducer(producerSequenceable);
    if (ret != HDF_SUCCESS) {
        IAM_LOGE("GetExecutorList fail");
        return FACE_AUTH_ERROR;
    }

    return FACE_AUTH_SUCCESS;
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
