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

#ifndef ISA_COMMAND_PROCESSOR
#define ISA_COMMAND_PROCESSOR

#include "face_auth_hdi.h"

#include "face_auth_executor_hdi.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
class ISaCommandProcessor {
public:
    ISaCommandProcessor() = default;
    virtual ~ISaCommandProcessor() = default;

    virtual UserAuth::ResultCode ProcessSaCommand(std::shared_ptr<FaceAuthExecutorHdi> executor,
        const SaCommand &command) = 0;
    virtual void OnHdiDisconnect(std::shared_ptr<FaceAuthExecutorHdi> executor) = 0;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // ISA_COMMAND_PROCESSOR