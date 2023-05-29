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

#include "gtest/gtest.h"

#include "sa_command_manager.h"

#include "iam_logger.h"
#include "iam_ptr.h"
#include "face_auth_hdi.h"
#include "mock_isa_command_processor.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class SaCommandManagerUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    SaCommandId getSaCommandId();

private:
    static int32_t saCommandId_;
};

int32_t SaCommandManagerUnitTest::saCommandId_ = 600000;

void SaCommandManagerUnitTest::SetUpTestCase()
{
}

void SaCommandManagerUnitTest::TearDownTestCase()
{
}

void SaCommandManagerUnitTest::SetUp()
{
}

void SaCommandManagerUnitTest::TearDown()
{
}

SaCommandId SaCommandManagerUnitTest::getSaCommandId()
{
    int32_t saCommandId = saCommandId_;
    saCommandId_++;
    IAM_LOGI("get command %{public}d", saCommandId);
    return static_cast<SaCommandId>(saCommandId);
}

HWTEST_F(SaCommandManagerUnitTest, SaCommandManagerUnitTest_001, TestSize.Level0)
{
    IAM_LOGI("begin SaCommandManagerUnitTest_001");
    std::vector<SaCommandId> commandIds;
    SaCommandManager::GetInstance().RegisterSaCommandProcessor(commandIds, nullptr);
    const std::vector<SaCommand> commands;
    auto ret = SaCommandManager::GetInstance().ProcessSaCommands(nullptr, commands);
    EXPECT_TRUE(ret == UserAuth::SUCCESS);
    SaCommandManager::GetInstance().OnHdiDisconnect(nullptr);
}

HWTEST_F(SaCommandManagerUnitTest, SaCommandManagerUnitTest_002, TestSize.Level0)
{
    IAM_LOGI("begin SaCommandManagerUnitTest_002");
    auto saCommandProcessor = Common::MakeShared<MockISaCommandProcessor>();
    ASSERT_TRUE(saCommandProcessor != nullptr);
    EXPECT_CALL(*saCommandProcessor, OnHdiDisconnect(_))
        .Times(Exactly(1))
        .WillOnce([](std::shared_ptr<FaceAuthExecutorHdi> executor) {
            IAM_LOGI("SaCommandManagerUnitTest_002 OnHdiDisconnect called");
            return;
        });
    std::vector<SaCommandId> commandIds = { getSaCommandId() };
    SaCommandManager::GetInstance().RegisterSaCommandProcessor(commandIds, saCommandProcessor);
    std::vector<SaCommand> commands;
    SaCommand saCommand = {
        .id = getSaCommandId(),
        .param = {}
    };
    commands.push_back(saCommand);
    IAM_LOGI("ProcessSaCommands %{public}d", saCommand.id);
    auto ret = SaCommandManager::GetInstance().ProcessSaCommands(nullptr, commands);
    EXPECT_TRUE(ret == UserAuth::GENERAL_ERROR);
    SaCommandManager::GetInstance().OnHdiDisconnect(nullptr);
    SaCommandManager::GetInstance().UnregisterSaCommandProcessor(commandIds, saCommandProcessor);
}

HWTEST_F(SaCommandManagerUnitTest, SaCommandManagerUnitTest_003, TestSize.Level0)
{
    IAM_LOGI("begin SaCommandManagerUnitTest_003");
    auto saCommandProcessor = Common::MakeShared<MockISaCommandProcessor>();
    ASSERT_TRUE(saCommandProcessor != nullptr);
    EXPECT_CALL(*saCommandProcessor, ProcessSaCommand(_, _))
        .Times(Exactly(1))
        .WillOnce([](std::shared_ptr<FaceAuthExecutorHdi> executor, const SaCommand &command) {
            IAM_LOGI("SaCommandManagerUnitTest_003 ProcessSaCommand called");
            return UserAuth::SUCCESS;
        });
    std::vector<SaCommandId> commandIds = { getSaCommandId() };
    SaCommandManager::GetInstance().RegisterSaCommandProcessor(commandIds, saCommandProcessor);
    std::vector<SaCommand> commands;
    SaCommand saCommand = {
        .id = commandIds[0],
        .param = {}
    };
    commands.push_back(saCommand);
    IAM_LOGI("ProcessSaCommands %{public}d", commandIds[0]);
    auto ret = SaCommandManager::GetInstance().ProcessSaCommands(nullptr, commands);
    EXPECT_TRUE(ret == UserAuth::SUCCESS);
    SaCommandManager::GetInstance().UnregisterSaCommandProcessor(commandIds, saCommandProcessor);
}

HWTEST_F(SaCommandManagerUnitTest, SaCommandManagerUnitTest_004, TestSize.Level0)
{
    IAM_LOGI("begin SaCommandManagerUnitTest_004");
    auto saCommandProcessor = Common::MakeShared<MockISaCommandProcessor>();
    ASSERT_TRUE(saCommandProcessor != nullptr);
    EXPECT_CALL(*saCommandProcessor, ProcessSaCommand(_, _))
        .Times(Exactly(1))
        .WillOnce([](std::shared_ptr<FaceAuthExecutorHdi> executor, const SaCommand &command) {
            IAM_LOGI("SaCommandManagerUnitTest_004 ProcessSaCommand called");
            return UserAuth::GENERAL_ERROR;
        });
    std::vector<SaCommandId> commandIds = { getSaCommandId() };
    SaCommandManager::GetInstance().RegisterSaCommandProcessor(commandIds, saCommandProcessor);
    std::vector<SaCommand> commands;
    SaCommand saCommand = {
        .id = commandIds[0],
        .param = {}
    };
    commands.push_back(saCommand);
    IAM_LOGI("ProcessSaCommands %{public}d", commandIds[0]);
    auto ret = SaCommandManager::GetInstance().ProcessSaCommands(nullptr, commands);
    EXPECT_TRUE(ret == UserAuth::GENERAL_ERROR);
    SaCommandManager::GetInstance().UnregisterSaCommandProcessor(commandIds, saCommandProcessor);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS