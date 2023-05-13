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

#ifndef SA_COMMAND_MANGER
#define SA_COMMAND_MANGER

#include <map>
#include <memory>
#include <set>
#include <shared_mutex>
#include <vector>

#include "face_auth_hdi.h"

#include "face_auth_executor_hdi.h"
#include "isa_command_processor.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
class SaCommandManager {
public:
    static SaCommandManager &GetInstance();

    void RegisterSaCommandProcessor(std::vector<SaCommandId> commandIds,
        std::shared_ptr<ISaCommandProcessor> processor);

    UserAuth::ResultCode ProcessSaCommands(std::shared_ptr<FaceAuthExecutorHdi> executor,
        const std::vector<SaCommand> &commands);
    void OnHdiDisconnect(std::shared_ptr<FaceAuthExecutorHdi> executor);

private:
    SaCommandManager() = default;
    ~SaCommandManager() = default;

    std::map<SaCommandId, std::set<std::shared_ptr<ISaCommandProcessor>>> commandId2Processors_;
    std::set<std::shared_ptr<ISaCommandProcessor>> processors_;
    std::shared_mutex mutex_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // SA_COMMAND_MANGER