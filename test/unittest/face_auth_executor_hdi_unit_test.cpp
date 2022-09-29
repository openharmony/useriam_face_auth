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

#include "hdf_base.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "face_auth_defines.h"
#include "face_auth_executor_hdi.h"
#include "mock_iexecute_callback.h"
#include "mock_iexecutor.h"

#define LOG_LABEL OHOS::UserIam::Common::LABEL_FACE_AUTH_SA

using namespace testing;
using namespace testing::ext;
using namespace OHOS::UserIam::Common;
namespace FaceHdi = OHOS::HDI::FaceAuth::V1_0;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
using IamResultCode = OHOS::UserIam::UserAuth::ResultCode;
using IamExecutorRole = OHOS::UserIam::UserAuth::ExecutorRole;
using IamExecutorInfo = OHOS::UserIam::UserAuth::ExecutorInfo;
using IamAuthType = OHOS::UserIam::UserAuth::AuthType;
using IamExecutorSecureLevel = OHOS::UserIam::UserAuth::ExecutorSecureLevel;
using IamPropertyMode = OHOS::UserIam::UserAuth::PropertyMode;
namespace {
static const std::map<HDF_STATUS, IamResultCode> RESULT_CODE_MAP = {
    {HDF_SUCCESS, IamResultCode::SUCCESS},
    {HDF_FAILURE, IamResultCode::GENERAL_ERROR},
    {HDF_ERR_TIMEOUT, IamResultCode::TIMEOUT},
    {HDF_ERR_QUEUE_FULL, IamResultCode::BUSY},
    {HDF_ERR_DEVICE_BUSY, IamResultCode::BUSY},
    {static_cast<HDF_STATUS>(HDF_ERR_DEVICE_BUSY - 1), IamResultCode::GENERAL_ERROR},
    {static_cast<HDF_STATUS>(HDF_SUCCESS + 1), IamResultCode::GENERAL_ERROR},
};
}

class FaceAuthExecutorHdiUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FaceAuthExecutorHdiUnitTest::SetUpTestCase()
{
}

void FaceAuthExecutorHdiUnitTest::TearDownTestCase()
{
}

void FaceAuthExecutorHdiUnitTest::SetUp()
{
}

void FaceAuthExecutorHdiUnitTest::TearDown()
{
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetExecutorInfo_001, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, GetExecutorInfo(_)).Times(Exactly(1)).WillOnce([](FaceHdi::ExecutorInfo &info) {
        info = {
            .executorRole = FaceHdi::ExecutorRole::ALL_IN_ONE,
            .authType = FaceHdi::FACE,
            .esl = FaceHdi::ExecutorSecureLevel::ESL0,
        };
        return HDF_SUCCESS;
    });
    FaceAuthExecutorHdi executorHdi(executorProxy);
    IamExecutorInfo info = {};
    auto ret = executorHdi.GetExecutorInfo(info);
    EXPECT_TRUE(info.executorRole == IamExecutorRole::ALL_IN_ONE);
    EXPECT_TRUE(info.authType == IamAuthType::FACE);
    EXPECT_TRUE(info.esl == IamExecutorSecureLevel::ESL0);
    EXPECT_TRUE(ret == IamResultCode::SUCCESS);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetExecutorInfo_002, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FaceHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = FaceHdi::ExecutorRole::ALL_IN_ONE,
                    .authType = FaceHdi::FACE,
                    .esl = FaceHdi::ExecutorSecureLevel::ESL0,
                };
                return static_cast<int32_t>(pair.first);
            });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetExecutorInfo_003, TestSize.Level0)
{
    static const std::map<FaceHdi::AuthType, pair<IamAuthType, IamResultCode>> data = {
        {FaceHdi::FACE, {IamAuthType::FACE, IamResultCode::SUCCESS}},
        {static_cast<FaceHdi::AuthType>(FaceHdi::FACE + 1),
            {IamAuthType::FACE, IamResultCode::GENERAL_ERROR}},
        {static_cast<FaceHdi::AuthType>(FaceHdi::FACE - 1),
            {IamAuthType::FACE, IamResultCode::GENERAL_ERROR}},
    };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FaceHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = FaceHdi::ExecutorRole::ALL_IN_ONE,
                    .authType = pair.first,
                    .esl = FaceHdi::ExecutorSecureLevel::ESL0,
                };
                return HDF_SUCCESS;
            });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.authType == pair.second.first);
        }
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetExecutorInfo_004, TestSize.Level0)
{
    static const std::map<FaceHdi::ExecutorRole, pair<IamExecutorRole, IamResultCode>> data = {
        {FaceHdi::ExecutorRole::COLLECTOR, {IamExecutorRole::COLLECTOR, IamResultCode::SUCCESS}},
        {FaceHdi::ExecutorRole::VERIFIER, {IamExecutorRole::VERIFIER, IamResultCode::SUCCESS}},
        {FaceHdi::ExecutorRole::ALL_IN_ONE, {IamExecutorRole::ALL_IN_ONE, IamResultCode::SUCCESS}},
        {static_cast<FaceHdi::ExecutorRole>(FaceHdi::ExecutorRole::COLLECTOR - 1),
            {IamExecutorRole::ALL_IN_ONE, IamResultCode::GENERAL_ERROR}},
        {static_cast<FaceHdi::ExecutorRole>(FaceHdi::ExecutorRole::ALL_IN_ONE + 1),
            {IamExecutorRole::ALL_IN_ONE, IamResultCode::GENERAL_ERROR}},
    };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FaceHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = pair.first,
                    .authType = FaceHdi::FACE,
                    .esl = FaceHdi::ExecutorSecureLevel::ESL0,
                };
                return HDF_SUCCESS;
            });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.executorRole == pair.second.first);
        }
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetExecutorInfo_005, TestSize.Level0)
{
    static const std::map<FaceHdi::ExecutorSecureLevel, pair<IamExecutorSecureLevel, IamResultCode>> data =
        {
            {FaceHdi::ExecutorSecureLevel::ESL0, {IamExecutorSecureLevel::ESL0, IamResultCode::SUCCESS}},
            {FaceHdi::ExecutorSecureLevel::ESL1, {IamExecutorSecureLevel::ESL1, IamResultCode::SUCCESS}},
            {FaceHdi::ExecutorSecureLevel::ESL2, {IamExecutorSecureLevel::ESL2, IamResultCode::SUCCESS}},
            {FaceHdi::ExecutorSecureLevel::ESL3, {IamExecutorSecureLevel::ESL3, IamResultCode::SUCCESS}},
            {static_cast<FaceHdi::ExecutorSecureLevel>(FaceHdi::ExecutorSecureLevel::ESL0 - 1),
                {IamExecutorSecureLevel::ESL3, IamResultCode::GENERAL_ERROR}},
            {static_cast<FaceHdi::ExecutorSecureLevel>(FaceHdi::ExecutorSecureLevel::ESL3 + 1),
                {IamExecutorSecureLevel::ESL3, IamResultCode::GENERAL_ERROR}},
        };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FaceHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = FaceHdi::ExecutorRole::ALL_IN_ONE,
                    .authType = FaceHdi::FACE,
                    .esl = pair.first,
                };
                return HDF_SUCCESS;
            });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.esl == pair.second.first);
        }
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetExecutorInfo_006, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    IamExecutorInfo info = {};
    auto ret = executorHdi.GetExecutorInfo(info);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetTemplateInfo_001, TestSize.Level0)
{
    const UserAuth::TemplateInfo data = {.executorType = 1,
        .freezingTime = 2,
        .remainTimes = 3,
        .extraInfo = {4, 5, 6}};
    auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, GetTemplateInfo(_, _))
        .Times(Exactly(1))
        .WillOnce([&data](uint64_t templateId, FaceHdi::TemplateInfo &info) {
            info = {.executorType = data.executorType,
                .lockoutDuration = data.freezingTime,
                .remainAttempts = data.remainTimes,
                .extraInfo = data.extraInfo};
            return HDF_SUCCESS;
        });
    FaceAuthExecutorHdi executorHdi(executorProxy);
    UserAuth::TemplateInfo info = {};
    auto ret = executorHdi.GetTemplateInfo(0, info);
    EXPECT_TRUE(ret == IamResultCode::SUCCESS);
    EXPECT_TRUE(info.executorType == data.executorType);
    EXPECT_TRUE(info.freezingTime == data.freezingTime);
    EXPECT_TRUE(info.remainTimes == data.remainTimes);
    EXPECT_TRUE(info.extraInfo == data.extraInfo);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetTemplateInfo_002, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetTemplateInfo(_, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t templateId, FaceHdi::TemplateInfo &info) { return pair.first; });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        UserAuth::TemplateInfo info = {};
        auto ret = executorHdi.GetTemplateInfo(0, info);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_GetTemplateInfo_003, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    UserAuth::TemplateInfo info = {};
    auto ret = executorHdi.GetTemplateInfo(0, info);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_OnRegisterFinish_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, OnRegisterFinish(_, _, _))
            .Times(Exactly(1))
            .WillOnce(
                [&pair](const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &frameworkPublicKey,
                    const std::vector<uint8_t> &extraInfo) { return pair.first; });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        UserAuth::TemplateInfo info = {};
        auto ret =
            executorHdi.OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_OnRegisterFinish_002, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    auto ret = executorHdi.OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Enroll_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Enroll(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
                          const sptr<FaceHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.Enroll(0, 0, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Enroll_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Enroll(_, _, _)).Times(Exactly(0));
    FaceAuthExecutorHdi executorHdi(executorProxy);
    auto ret = executorHdi.Enroll(0, 0, std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Enroll_003, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi.Enroll(0, 0, std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Authenticate_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Authenticate(_, _, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint64_t> &templateIdList,
                          const std::vector<uint8_t> &extraInfo,
                          const sptr<FaceHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.Authenticate(0, 0, std::vector<uint64_t>(), std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Authenticate_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Authenticate(_, _, _, _)).Times(Exactly(0));
    FaceAuthExecutorHdi executorHdi(executorProxy);
    auto ret = executorHdi.Authenticate(0, 0, std::vector<uint64_t>(), std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Authenticate_003, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi.Authenticate(0, 0, std::vector<uint64_t>(), std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}
HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Identify_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Identify(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
                          const sptr<FaceHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.Identify(0, 0, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Identify_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Identify(_, _, _)).Times(Exactly(0));
    FaceAuthExecutorHdi executorHdi(executorProxy);
    auto ret = executorHdi.Identify(0, 0, std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Identify_003, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi.Identify(0, 0, std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Delete_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Delete(_))
            .Times(Exactly(1))
            .WillOnce([&pair](const std::vector<uint64_t> &templateIdList) { return pair.first; });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        auto ret = executorHdi.Delete(std::vector<uint64_t>());
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Delete_002, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    auto ret = executorHdi.Delete(std::vector<uint64_t>());
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Cancel_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Cancel(_)).Times(Exactly(1)).WillOnce([&pair](uint64_t scheduleId) {
            return pair.first;
        });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        auto ret = executorHdi.Cancel(0);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_Cancel_002, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    auto ret = executorHdi.Cancel(0);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_SendCommand_001, TestSize.Level0)
{
    FaceAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret =
        executorHdi.SendCommand(IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_SendCommand_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, SendCommand(_, _, _)).Times(Exactly(0));
    FaceAuthExecutorHdi executorHdi(executorProxy);

    auto ret = executorHdi.SendCommand(IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_SendCommand_003, TestSize.Level0)
{
    static const std::map<IamPropertyMode, pair<FaceHdi::CommandId, IamResultCode>> data = {
        {IamPropertyMode::PROPERTY_MODE_FREEZE,
            {FaceHdi::CommandId::LOCK_TEMPLATE, IamResultCode::SUCCESS}},
        {IamPropertyMode::PROPERTY_MODE_UNFREEZE,
            {FaceHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::SUCCESS}},
        {static_cast<IamPropertyMode>(IamPropertyMode::PROPERTY_MODE_FREEZE - 1),
            {FaceHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS}},
        {static_cast<IamPropertyMode>(IamPropertyMode::PROPERTY_MODE_UNFREEZE + 1),
            {FaceHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS}},
        {static_cast<IamPropertyMode>(FaceHdi::VENDOR_COMMAND_BEGIN),
            {FaceHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS}},
        {static_cast<IamPropertyMode>(FaceHdi::VENDOR_COMMAND_BEGIN + 1),
            {static_cast<FaceHdi::CommandId>(FaceHdi::VENDOR_COMMAND_BEGIN + 1), IamResultCode::SUCCESS}}};
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        if (pair.second.second == IamResultCode::SUCCESS) {
            EXPECT_CALL(*executorProxy, SendCommand(_, _, _))
                .Times(Exactly(1))
                .WillOnce([&pair](int32_t commandId, const std::vector<uint8_t> &extraInfo,
                              const sptr<FaceHdi::IExecutorCallback> &callbackObj) {
                    EXPECT_TRUE(commandId == pair.second.first);
                    return HDF_SUCCESS;
                });
        } else {
            EXPECT_CALL(*executorProxy, SendCommand(_, _, _)).Times(Exactly(0));
        }
        FaceAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.SendCommand(pair.first, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second.second);
    }
}

HWTEST_F(FaceAuthExecutorHdiUnitTest, FaceAuthExecutorHdi_SendCommand_004, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FaceHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, SendCommand(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](int32_t commandId, const std::vector<uint8_t> &extraInfo,
                          const sptr<FaceHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FaceAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.SendCommand(
            IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
