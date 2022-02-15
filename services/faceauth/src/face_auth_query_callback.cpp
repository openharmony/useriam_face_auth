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
#include "face_auth_manager.h"
#include "face_auth_log_wrapper.h"
#include "coauth_info_define.h"
#include "face_auth_ca.h"
#include "face_auth_query_callback.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
void FaceAuthQueryCallback::OnResult(uint32_t resultCode)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "resultCode = %{public}u.", resultCode);
    std::shared_ptr<FaceAuthManager> manager = FaceAuthManager::GetInstance();
    if (manager != nullptr) {
        if (resultCode != EXECUTOR_REGISTER) {
            manager->RegisterExecutor();
        }
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
