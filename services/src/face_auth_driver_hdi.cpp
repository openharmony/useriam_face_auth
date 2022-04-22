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

#include <cstdint>

#include "face_auth_defines.h"
#include "face_auth_executor_hdi.h"
#include "iam_logger.h"
#include "iam_ptr.h"
#include "iauth_executor_hdi.h"
#include "v1_0/iface_auth_interface.h"

#define LOG_LABEL UserIAM::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
void FaceAuthDriverHdi::GetExecutorList(std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> &executorList)
{
    auto faceIf = FaceHdi::IFaceAuthInterface::Get();
    if (faceIf == nullptr) {
        IAM_LOGE("IFaceAuthInterface is null");
        return;
    }

    std::vector<sptr<FaceHdi::IExecutor>> iExecutorList;
    faceIf->GetExecutorList(iExecutorList);
    for (const auto &iExecutor : iExecutorList) {
        auto executor = Common::MakeShared<FaceAuthExecutorHdi>(iExecutor);
        if (executor == nullptr) {
            IAM_LOGE("make share failed");
            continue;
        }
        executorList.push_back(executor);
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
