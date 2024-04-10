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

#ifndef FACE_AUTH_DRIVER_HDI
#define FACE_AUTH_DRIVER_HDI

#include <vector>

#include "iremote_broker.h"
#include "nocopyable.h"

#include "iam_executor_iauth_driver_hdi.h"
#include "iam_executor_iauth_executor_hdi.h"

#include "face_auth_all_in_one_executor_hdi.h"
#include "face_auth_interface_adapter.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
class FaceAuthDriverHdi : public UserAuth::IAuthDriverHdi, public NoCopyable {
public:
    explicit FaceAuthDriverHdi(const std::shared_ptr<FaceAuthInterfaceAdapter> faceAuthInterfaceAdapter);
    ~FaceAuthDriverHdi() override = default;

    void GetExecutorList(std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> &executorList) override;
    void OnHdiDisconnect() override;

    int32_t SetBufferProducer(sptr<IBufferProducer> &producer);

private:
    static std::mutex mutex_;
    const std::shared_ptr<FaceAuthInterfaceAdapter> faceAuthInterfaceAdapter_;
    std::vector<std::shared_ptr<FaceAuthAllInOneExecutorHdi>> faceAuthExecutorList_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_DRIVER_HDI