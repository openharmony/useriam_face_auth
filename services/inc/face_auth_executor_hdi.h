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

#ifndef FACE_AUTH_EXECUTOR_HDI_H
#define FACE_AUTH_EXECUTOR_HDI_H

#include <cstdint>
#include <map>
#include <vector>

#include "nocopyable.h"

#include "framework_types.h"
#include "iauth_executor_hdi.h"
#include "iexecute_callback.h"
#include "v1_0/executor_proxy.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace FaceHdi = OHOS::HDI::FaceAuth::V1_0;
class FaceAuthExecutorHdi : public UserAuth::IAuthExecutorHdi, public NoCopyable {
public:
    explicit FaceAuthExecutorHdi(sptr<FaceHdi::IExecutor> executorProxy);
    ~FaceAuthExecutorHdi() override = default;

    UserIAM::ResultCode GetExecutorInfo(UserIAM::ExecutorInfo &info) override;
    UserIAM::ResultCode GetTemplateInfo(uint64_t templateId, UserAuth::TemplateInfo &info) override;
    UserIAM::ResultCode OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo) override;
    UserIAM::ResultCode Enroll(uint64_t scheduleId, uint32_t tokenId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserIAM::ResultCode Authenticate(uint64_t scheduleId, uint32_t tokenId,
        const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserIAM::ResultCode Identify(uint64_t scheduleId, uint32_t tokenId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserIAM::ResultCode Delete(const std::vector<uint64_t> &templateIdList) override;
    UserIAM::ResultCode Cancel(uint64_t scheduleId) override;
    UserIAM::ResultCode SendCommand(UserAuth::AuthPropertyMode commandId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;

private:
    UserIAM::ResultCode MoveHdiExecutorInfo(FaceHdi::ExecutorInfo &in, UserIAM::ExecutorInfo &out);
    void MoveHdiTemplateInfo(FaceHdi::TemplateInfo &in, UserAuth::TemplateInfo &out);
    UserIAM::ResultCode ConvertCommandId(const UserAuth::AuthPropertyMode in, FaceHdi::CommandId &out);
    UserIAM::ResultCode ConvertAuthType(const FaceHdi::AuthType in, UserIAM::AuthType &out);
    UserIAM::ResultCode ConvertExecutorRole(const FaceHdi::ExecutorRole in, UserIAM::ExecutorRole &out);
    UserIAM::ResultCode ConvertExecutorSecureLevel(
        const FaceHdi::ExecutorSecureLevel in, UserIAM::ExecutorSecureLevel &out);
    UserIAM::ResultCode ConvertResultCode(const int32_t in);

    sptr<FaceHdi::IExecutor> executorProxy_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACE_AUTH_EXECUTOR_HDI_H