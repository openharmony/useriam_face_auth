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

#ifndef FACEAUTH_SERVICES_INCLUDE_IFACE_AUTH_CA_H
#define FACEAUTH_SERVICES_INCLUDE_IFACE_AUTH_CA_H

#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class IFaceAuthCa {
public:
    virtual int32_t Prepare(std::unique_ptr<PrepareParam> param) = 0;
    virtual int32_t PreEnroll(int64_t &challenge, int32_t type) = 0;
    virtual int32_t PostEnroll(int32_t type) = 0;
    virtual int32_t DeleteUser(uint32_t uid) = 0;
    virtual int32_t DeleteTemolate(int32_t faceId) = 0;
    virtual int32_t DeleteTemplateStep2() = 0;
    virtual int32_t SetActiveGroup(const uint32_t uid, std::list<int32_t> &faceInfolist) = 0;
    virtual int32_t VerifyAuthToken(std::unique_ptr<uint8_t[]> authToken, uint32_t length, int32_t type) = 0;
    virtual int32_t GetAuthToken(std::unique_ptr<uint8_t[]> &authToken, uint32_t &len) = 0;
    virtual int32_t SetChallenge(int64_t challenge) = 0;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_SERVICES_INCLUDE_IFACE_AUTH_CA_H