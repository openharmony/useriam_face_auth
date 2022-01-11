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

#include "face_auth_bms_adapter.h"
#include "constant.h"
#include "ipc_skeleton.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "faceauth_log_wrapper.h"
#include "sa_mgr_client.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthBmsAdapter::mutex_;
std::shared_ptr<FaceAuthBmsAdapter> FaceAuthBmsAdapter::instance_ = nullptr;
std::shared_ptr<FaceAuthBmsAdapter> FaceAuthBmsAdapter::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthBmsAdapter>();
        }
    }
    return instance_;
}
sptr<AppExecFwk::IBundleMgr> FaceAuthBmsAdapter::GetBundleManager()
{
    auto bundleObj = OHOS::DelayedSingleton<AAFwk::SaMgrClient>::GetInstance()->GetSystemAbility(
        Constant::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        FACEAUTH_LABEL_LOGE("failed to get bundle manager service.");
        return nullptr;
    }
    iBundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    if (iBundleManager_ == nullptr) {
        FACEAUTH_LABEL_LOGI("iBundleMgr is null");
        return nullptr;
    }
    return iBundleManager_;
}

std::string FaceAuthBmsAdapter::GetCallingBundleName()
{
    FACEAUTH_LABEL_LOGI("FaceAuthBmsAdapter GetCallingBundleName start");
    int32_t uid = IPCSkeleton::GetCallingUid();
    FACEAUTH_LABEL_LOGI("uid is %{public}d", uid);
    if (iBundleManager_ == nullptr) {
        iBundleManager_ = GetBundleManager();
    }
    std::string bundleName;
    bool result = iBundleManager_->GetBundleNameForUid(uid, bundleName);
    FACEAUTH_LABEL_LOGI("bundleName is %{public}s", bundleName.c_str());
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    if (!result) {
        FACEAUTH_LABEL_LOGE("cannot get bundle name by uid %{public}d", uid);
        return "";
    }
    FACEAUTH_LABEL_LOGI("FaceAuthBmsAdapter GetCallingBundleName end");
    return bundleName;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
