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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_SERVICE_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_SERVICE_H
#include <mutex>
#include "system_ability.h"
#include "system_ability_definition.h"
#include "singleton.h"
#include "face_auth_manager.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthService : public SystemAbility {
public:
    DECLEAR_SYSTEM_ABILITY(FaceAuthService);
    static FaceAuthService *GetInstance();
    FaceAuthService();
    virtual ~FaceAuthService() override;
public:
    virtual void OnStart() override;
    virtual void OnStop() override;
    void Start();
    void ReRegister();
private:
    static FaceAuthService *instance_;
    static std::shared_ptr<FaceAuthManager> manager_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_SERVICE_H