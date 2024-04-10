/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "iam_logger.h"
#include "iam_ptr.h"

#include "screen_brightness_manager.h"
#include "service_ex_manager.h"

using namespace testing;
using namespace testing::ext;

#define LOG_TAG "FACE_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthScreenBrightnessTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FaceAuthScreenBrightnessTest::SetUpTestCase()
{
}

void FaceAuthScreenBrightnessTest::TearDownTestCase()
{
}

void FaceAuthScreenBrightnessTest::SetUp()
{
}

void FaceAuthScreenBrightnessTest::TearDown()
{
}

HWTEST_F(FaceAuthScreenBrightnessTest, FaceAuthScreenBrightnessTest_001, TestSize.Level0)
{
    auto acquireRet = ServiceExManager::GetInstance().Acquire();
    EXPECT_TRUE(acquireRet == UserAuth::SUCCESS);

    auto task = ServiceExManager::GetInstance().GetScreenBrightnessTask();
    EXPECT_TRUE(task != nullptr);

    task->RegisterDestructCallback([]() {
        IAM_LOGI("task destruct");
        ServiceExManager::GetInstance().Release();
    });
    task->Start();
    IAM_LOGI("start brightness increase task");
    task->Stop();
    IAM_LOGI("end brightness increase task");
    task = nullptr;
}

HWTEST_F(FaceAuthScreenBrightnessTest, FaceAuthScreenBrightnessTest_002, TestSize.Level0)
{
    auto manager = ScreenBrightnessManager::GetInstance();
    EXPECT_TRUE(manager != nullptr);
    auto executor = Common::MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    EXPECT_TRUE(executor != nullptr);

    SaCommand beginCommand = { SaCommandId::BEGIN_SCREEN_BRIGHTNESS_INCREASE };
    SaCommand endCommand = { SaCommandId::END_SCREEN_BRIGHTNESS_INCREASE };
    auto result = manager->ProcessSaCommand(executor, beginCommand);
    EXPECT_TRUE(result == UserAuth::SUCCESS);
    IAM_LOGI("start brightness increase task");
    result = manager->ProcessSaCommand(executor, endCommand);
    EXPECT_TRUE(result == UserAuth::SUCCESS);
    IAM_LOGI("end brightness increase task");
}

} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
