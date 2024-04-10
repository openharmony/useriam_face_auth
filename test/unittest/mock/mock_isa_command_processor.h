/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef MOCK_ISA_COMMAND_PROCESSOR_H
#define MOCK_ISA_COMMAND_PROCESSOR_H

#include "gmock/gmock.h"

#include "isa_command_processor.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class MockISaCommandProcessor : public ISaCommandProcessor {
public:
    virtual ~MockISaCommandProcessor() = default;

    MOCK_METHOD2(ProcessSaCommand,
        UserAuth::ResultCode(std::shared_ptr<FaceAuthAllInOneExecutorHdi> executor, const SaCommand &command));
    MOCK_METHOD1(OnHdiDisconnect, void(std::shared_ptr<FaceAuthAllInOneExecutorHdi> executor));
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // MOCK_ISA_COMMAND_PROCESSOR_H