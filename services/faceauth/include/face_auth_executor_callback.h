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

#ifndef FACE_AUTH_EXECUTOR_CALLBACK_H
#define FACE_AUTH_EXECUTOR_CALLBACK_H

#include "face_auth_defines.h"
#include "executor_callback.h"
#include "auth_attributes.h"
#include "auth_executor.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
using pAuthAttributes = std::shared_ptr<AuthResPool::AuthAttributes>;
using pAuthMessage = std::shared_ptr<AuthResPool::AuthMessage>;

class FaceAuthExecutorCallback : public AuthResPool::ExecutorCallback {
public:
    FaceAuthExecutorCallback() = default;
    virtual ~FaceAuthExecutorCallback() = default;
    int32_t OnBeginExecute(uint64_t scheduleId, std::vector<uint8_t> &publicKey, pAuthAttributes commandAttrs) override;
    int32_t OnEndExecute(uint64_t scheduleId, pAuthAttributes consumerAttr) override;
    int32_t OnSetProperty(pAuthAttributes properties) override;
    void OnMessengerReady(const sptr<AuthResPool::IExecutorMessenger> &messenger) override;
    int32_t OnGetProperty(std::shared_ptr<AuthResPool::AuthAttributes> conditions,
        std::shared_ptr<AuthResPool::AuthAttributes> values) override;
};
} // namespace FaceAuth
} // namespace userIAM
} // namespace OHOS
#endif // FACE_AUTH_EXECUTOR_CALLBACK_H
