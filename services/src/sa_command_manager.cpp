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

#include "sa_command_manager.h"

#include "iam_check.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
SaCommandManager &SaCommandManager::GetInstance()
{
    static SaCommandManager manager;
    return manager;
}

void SaCommandManager::RegisterSaCommandProcessor(std::vector<SaCommandId> commandIds,
    std::shared_ptr<ISaCommandProcessor> processor)
{
    IF_FALSE_LOGE_AND_RETURN(processor != nullptr);

    std::unique_lock<std::shared_mutex> lock(mutex_);

    processors_.insert(processor);

    for (const auto &commandId : commandIds) {
        if (commandId2Processors_.find(commandId) == commandId2Processors_.end()) {
            commandId2Processors_[commandId] = std::set<std::shared_ptr<ISaCommandProcessor>>();
        }

        commandId2Processors_[commandId].insert(processor);
    }
}

void SaCommandManager::UnregisterSaCommandProcessor(std::vector<SaCommandId> commandIds,
    std::shared_ptr<ISaCommandProcessor> processor)
{
    IF_FALSE_LOGE_AND_RETURN(processor != nullptr);

    std::unique_lock<std::shared_mutex> lock(mutex_);

    processors_.erase(processor);

    for (const auto &commandId : commandIds) {
        if (commandId2Processors_.find(commandId) == commandId2Processors_.end()) {
            continue;
        }

        commandId2Processors_[commandId].erase(processor);
    }
}

UserAuth::ResultCode SaCommandManager::ProcessSaCommands(std::shared_ptr<FaceAuthExecutorHdi> executor,
    const std::vector<SaCommand> &commands)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    for (const auto &command : commands) {
        auto it = commandId2Processors_.find(command.id);
        IF_FALSE_LOGE_AND_RETURN_VAL(it != commandId2Processors_.end(), UserAuth::GENERAL_ERROR);
        for (const auto &processor : commandId2Processors_[command.id]) {
            IF_FALSE_LOGE_AND_RETURN_VAL(processor != nullptr, UserAuth::GENERAL_ERROR);
            UserAuth::ResultCode result = processor->ProcessSaCommand(executor, command);
            IF_FALSE_LOGE_AND_RETURN_VAL(result == UserAuth::SUCCESS, UserAuth::GENERAL_ERROR);
        }
    }

    return UserAuth::SUCCESS;
}

void SaCommandManager::OnHdiDisconnect(std::shared_ptr<FaceAuthExecutorHdi> executor)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    for (const auto &processor : processors_) {
        IF_FALSE_LOGE_AND_RETURN(processor != nullptr);
        processor->OnHdiDisconnect(executor);
    }

    return;
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS