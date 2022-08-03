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

#ifndef FACE_AUTH_INTERFACE_ADAPTER
#define FACE_AUTH_INTERFACE_ADAPTER

#include "v1_0/iface_auth_interface.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
using namespace OHOS::HDI::FaceAuth::V1_0;
class FaceAuthInterfaceAdapter {
public:
    explicit FaceAuthInterfaceAdapter() = default;
    virtual ~FaceAuthInterfaceAdapter() = default;

    virtual sptr<IFaceAuthInterface> Get();
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_INTERFACE_ADAPTER