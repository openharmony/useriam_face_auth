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

#ifndef CONSTANT_H
#define CONSTANT_H

#include <algorithm>
#include <string>
#include <map>

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class Constant {
public:
    /**
     * Code and errorCode translate into tipCode.
     */
    const static std::map<std::string, int32_t> TIP_CODE_MAP;

    /**
     * Code and errorCode translate into tipInfo.
     */
    const static std::map<std::string, std::string> TIP_INFO_MAP;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS
#endif  // PERMISSION_BASE_SERVICE_H