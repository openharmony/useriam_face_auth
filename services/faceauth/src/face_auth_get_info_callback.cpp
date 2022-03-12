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
#include "face_auth_get_info_callback.h"
#include "face_auth_log_wrapper.h"
#include "face_auth_ca.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
void FaceAuthGetInfoCallback::OnGetInfo(std::vector<UserIDM::CredentialInfo> &info)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run, credentialInfo length %{public}zu.", __PRETTY_FUNCTION__,
        info.size());
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA instance is null");
        return;
    }
    std::vector<uint64_t> templateIdList;
    for (auto infoItem : info) {
        templateIdList.push_back(infoItem.templateId);
    }
    faceAuthCA->VerifyTemplateData(templateIdList);
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
