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

#include "ibuffer_producer.h"

#include "iam_executor_framework_types.h"
#include "iam_executor_iauth_executor_hdi.h"
#include "iam_executor_iexecute_callback.h"
#include "v1_0/iexecutor.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace FaceHdi = OHOS::HDI::FaceAuth::V1_0;
namespace UserAuth = OHOS::UserIam::UserAuth;
class FaceAuthExecutorHdi : public UserAuth::IAuthExecutorHdi, public NoCopyable {
public:
    explicit FaceAuthExecutorHdi(sptr<FaceHdi::IExecutor> executorProxy);
    ~FaceAuthExecutorHdi() override = default;

    UserIam::UserAuth::ResultCode GetExecutorInfo(UserIam::UserAuth::ExecutorInfo &info) override;
    UserIam::UserAuth::ResultCode GetTemplateInfo(uint64_t templateId, UserAuth::TemplateInfo &info) override;
    UserIam::UserAuth::ResultCode OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo) override;
    UserIam::UserAuth::ResultCode Enroll(uint64_t scheduleId, uint32_t tokenId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserIam::UserAuth::ResultCode Authenticate(uint64_t scheduleId, uint32_t tokenId,
        const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserIam::UserAuth::ResultCode Identify(uint64_t scheduleId, uint32_t tokenId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserIam::UserAuth::ResultCode Delete(const std::vector<uint64_t> &templateIdList) override;
    UserIam::UserAuth::ResultCode Cancel(uint64_t scheduleId) override;
    UserIam::UserAuth::ResultCode SendCommand(UserIam::UserAuth::PropertyMode commandId,
        const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    int32_t SetBufferProducer(sptr<IBufferProducer> &producer);

private:
    UserIam::UserAuth::ResultCode MoveHdiExecutorInfo(FaceHdi::ExecutorInfo &in, UserIam::UserAuth::ExecutorInfo &out);
    void MoveHdiTemplateInfo(FaceHdi::TemplateInfo &in, UserAuth::TemplateInfo &out);
    UserIam::UserAuth::ResultCode ConvertCommandId(const UserIam::UserAuth::PropertyMode in, FaceHdi::CommandId &out);
    UserIam::UserAuth::ResultCode ConvertAuthType(const FaceHdi::AuthType in, UserIam::UserAuth::AuthType &out);
    UserIam::UserAuth::ResultCode ConvertExecutorRole(
        const FaceHdi::ExecutorRole in, UserIam::UserAuth::ExecutorRole &out);
    UserIam::UserAuth::ResultCode ConvertExecutorSecureLevel(
        const FaceHdi::ExecutorSecureLevel in, UserIam::UserAuth::ExecutorSecureLevel &out);
    UserIam::UserAuth::ResultCode ConvertResultCode(const int32_t in);

    sptr<FaceHdi::IExecutor> executorProxy_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_EXECUTOR_HDI_H