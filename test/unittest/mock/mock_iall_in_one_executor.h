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

#ifndef MOCK_IEXECUTOR_H
#define MOCK_IEXECUTOR_H

#include "gmock/gmock.h"

#include "face_auth_hdi.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class MockIAllInOneExecutor : public IAllInOneExecutor {
public:
    virtual ~MockIAllInOneExecutor() = default;

    MOCK_METHOD1(GetExecutorInfo, int32_t(ExecutorInfo &executorInfo));
    MOCK_METHOD3(OnRegisterFinish,
        int32_t(const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &frameworkPublicKey,
            const std::vector<uint8_t> &extraInfo));
    MOCK_METHOD3(Enroll,
        int32_t(uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
            const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD4(Authenticate,
        int32_t(uint64_t scheduleId, const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &extraInfo,
            const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD3(Identify,
        int32_t(uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
            const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD1(Delete, int32_t(const std::vector<uint64_t> &templateIdList));
    MOCK_METHOD1(Cancel, int32_t(uint64_t scheduleId));
    MOCK_METHOD3(SendCommand,
        int32_t(int32_t commandId, const std::vector<uint8_t> &extraInfo, const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD3(GetProperty,
        int32_t(const std::vector<uint64_t> &templateIdList, const std::vector<int32_t> &propertyTypes,
            Property &property));
    MOCK_METHOD1(SetCachedTemplates, int32_t(const std::vector<uint64_t> &templateIdList));
    MOCK_METHOD1(RegisterSaCommandCallback, int32_t(const sptr<ISaCommandCallback> &callbackObj));
    MOCK_METHOD3(SendMessage, int32_t(uint64_t scheduleId, int32_t srcRole, const std::vector<uint8_t> &msg));
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // MOCK_IEXECUTOR_H