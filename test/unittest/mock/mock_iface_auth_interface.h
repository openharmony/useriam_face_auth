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

#ifndef MOCK_IFACE_AUTH_INTERFACE_H
#define MOCK_IFACE_AUTH_INTERFACE_H

#include "gmock/gmock.h"

#include "v1_0/iface_auth_interface.h"

namespace OHOS {
namespace HDI {
namespace FaceAuth {
namespace V1_0 {
using namespace OHOS;
using namespace OHOS::HDI;

class MockIFaceAuthInterface : public IFaceAuthInterface {
public:
    virtual ~MockIFaceAuthInterface() = default;

    MOCK_METHOD1(GetExecutorList, int32_t(std::vector<sptr<IExecutor>> &executorList));
};
} // namespace V1_0
} // namespace FaceAuth
} // namespace HDI
} // namespace OHOS

#endif // MOCK_IFACE_AUTH_INTERFACE_MOCK_H