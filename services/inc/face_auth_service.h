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
#ifndef FACE_AUTH_SERVICE_H
#define FACE_AUTH_SERVICE_H

#include <cstdint>
#include <mutex>

#include "nocopyable.h"
#include "surface.h"
#include "system_ability.h"

#include "face_auth_stub.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthService : public SystemAbility, public FaceAuthStub {
    DECLEAR_SYSTEM_ABILITY(FaceAuthService);

public:
    FaceAuthService();
    ~FaceAuthService() override = default;
    static std::shared_ptr<FaceAuthService> GetInstance();

    void OnStart() override;
    void OnStop() override;
    int32_t SetBufferProducer(sptr<IBufferProducer> &producer) override;
private:
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthService> instance_;
    void StartDriverManager();
    bool IsPermissionGranted(const std::string &permission);
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_SERVICE_H
