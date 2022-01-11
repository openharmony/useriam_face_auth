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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CA_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CA_H

#include <fstream>
#include <list>
#include <iostream>
#include <stdint.h>
#include <mutex>
#include "iface_auth.h"
#include "iface_auth_ca.h"
#include "mock_common.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthCa : public IFaceAuthCa {
public:
    FaceAuthCa();
    virtual ~FaceAuthCa();
    static std::shared_ptr<FaceAuthCa> GetInstance();
    int32_t Prepare(std::unique_ptr<PrepareParam> param);
    int32_t PreEnroll(int64_t &challenge, int32_t type);
    int32_t PostEnroll(int32_t type);
    int32_t DeleteUser(uint32_t uid);
    int32_t DeleteTemolate(int32_t faceId);
    int32_t DeleteTemplateStep2();
    int32_t SetActiveGroup(const uint32_t uid, std::list<int32_t> &faceInfolist);
    int32_t VerifyAuthToken(std::unique_ptr<uint8_t[]> authToken, uint32_t length, int32_t type);
    int32_t GetAuthToken(std::unique_ptr<uint8_t[]> &authToken, uint32_t &len);
    int32_t SetChallenge(int64_t challenge);

private:
    void ReadPostFile();
    void CheckPostFile(std::string s);
    void CheckPreFile(std::string s);
    void ReadPreFile();
    void ReadFile();
    void CheckFile(std::string s);
    uint64_t challenge_;
    bool isInitFail_;
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthCa> instance_;
    std::list<int32_t> EnrollList_;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CA_H
