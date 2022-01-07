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

#ifndef FACEAUTH_KITS_INCLUDE_FACEAUTH_AUTH_MANAGER_H
#define FACEAUTH_KITS_INCLUDE_FACEAUTH_AUTH_MANAGER_H

#include <iostream>
#include "constant.h"
#include "face_auth_innerkit.h"
#include "face_auth_defines.h"
#include "on_faceauth.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class UseriamAuthKit {
public:
    static int32_t Execute(
        const AuthParam &param, const std::string &type, const std::string &level, const sptr<OnFaceAuth> &callback);

    static int32_t Cancel(const uint64_t reqId, const sptr<OnFaceAuth> &callback);

    static int32_t CheckAvailability(const std::string &type, const std::string &level);
    static Tip GetTips()
    {
        Tip t;
        t.errorCode = 1;
        return t;
    };
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_KITS_INCLUDE_FACEAUTH_AUTH_MANAGER_H