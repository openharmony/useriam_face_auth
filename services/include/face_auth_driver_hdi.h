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

#include <iauth_driver_hdi.h>
#include <vector>

#include "iauth_executor_hdi.h"
#include "iremote_broker.h"
#include "nocopyable.h"
#include "v1_0/face_auth_interface_proxy.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace FaceHdi = OHOS::HDI::FaceAuth::V1_0;
class FaceAuthDriverHdi : public UserAuth::IAuthDriverHdi, public NoCopyable {
public:
    FaceAuthDriverHdi() = default;
    virtual ~FaceAuthDriverHdi() = default;

    void GetExecutorList(std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> &executorList);
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACE_AUTH_DRIVER_HDI