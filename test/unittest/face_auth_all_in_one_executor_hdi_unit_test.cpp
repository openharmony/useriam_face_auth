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

#include "face_auth_all_in_one_executor_hdi.h"
#include "face_auth_defines.h"
#include "mock_iall_in_one_executor.h"
#include "mock_iexecute_callback.h"

#define LOG_TAG "FACE_AUTH_SA"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::UserIam::Common;

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
    { HDF_SUCCESS, IamResultCode::SUCCESS },
    { HDF_FAILURE, IamResultCode::GENERAL_ERROR },
    { HDF_ERR_TIMEOUT, IamResultCode::TIMEOUT },
    { HDF_ERR_QUEUE_FULL, IamResultCode::BUSY },
    { HDF_ERR_DEVICE_BUSY, IamResultCode::BUSY },
    { static_cast<HDF_STATUS>(HDF_ERR_DEVICE_BUSY - 1), IamResultCode::GENERAL_ERROR },
    { static_cast<HDF_STATUS>(HDF_SUCCESS + 1), IamResultCode::GENERAL_ERROR },
};
}

class FaceAuthAllInOneExecutorHdiUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FaceAuthAllInOneExecutorHdiUnitTest::SetUpTestCase()
{
}

void FaceAuthAllInOneExecutorHdiUnitTest::TearDownTestCase()
{
}

void FaceAuthAllInOneExecutorHdiUnitTest::SetUp()
{
}

void FaceAuthAllInOneExecutorHdiUnitTest::TearDown()
{
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetExecutorInfo_001, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, GetExecutorInfo(_)).Times(Exactly(1)).WillOnce([](ExecutorInfo &info) {
        info = {
            .executorRole = ExecutorRole::ALL_IN_ONE,
            .authType = AuthType::FACE,
            .esl = ExecutorSecureLevel::ESL0,
        };
        return HDF_SUCCESS;
    });
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
    IamExecutorInfo info = {};
    auto ret = executorHdi->GetExecutorInfo(info);
    EXPECT_TRUE(info.executorRole == IamExecutorRole::ALL_IN_ONE);
    EXPECT_TRUE(info.authType == IamAuthType::FACE);
    EXPECT_TRUE(info.esl == IamExecutorSecureLevel::ESL0);
    EXPECT_TRUE(ret == IamResultCode::SUCCESS);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetExecutorInfo_002, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_)).Times(Exactly(1)).WillOnce([&pair](ExecutorInfo &info) {
            info = {
                .executorRole = ExecutorRole::ALL_IN_ONE,
                .authType = AuthType::FACE,
                .esl = ExecutorSecureLevel::ESL0,
            };
            return static_cast<int32_t>(pair.first);
        });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi->GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetExecutorInfo_003, TestSize.Level0)
{
    static const std::map<AuthType, pair<IamAuthType, IamResultCode>> data = {
        { AuthType::FACE, { IamAuthType::FACE, IamResultCode::SUCCESS } },
        { static_cast<AuthType>(AuthType::FACE + 1), { IamAuthType::FACE, IamResultCode::GENERAL_ERROR } },
        { static_cast<AuthType>(AuthType::FACE - 1), { IamAuthType::FACE, IamResultCode::GENERAL_ERROR } },
    };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_)).Times(Exactly(1)).WillOnce([&pair](ExecutorInfo &info) {
            info = {
                .executorRole = ExecutorRole::ALL_IN_ONE,
                .authType = pair.first,
                .esl = ExecutorSecureLevel::ESL0,
            };
            return HDF_SUCCESS;
        });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi->GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.authType == pair.second.first);
        }
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetExecutorInfo_004, TestSize.Level0)
{
    static const std::map<ExecutorRole, pair<IamExecutorRole, IamResultCode>> data = {
        { ExecutorRole::COLLECTOR, { IamExecutorRole::COLLECTOR, IamResultCode::SUCCESS } },
        { ExecutorRole::VERIFIER, { IamExecutorRole::VERIFIER, IamResultCode::SUCCESS } },
        { ExecutorRole::ALL_IN_ONE, { IamExecutorRole::ALL_IN_ONE, IamResultCode::SUCCESS } },
        { static_cast<ExecutorRole>(ExecutorRole::COLLECTOR - 1),
            { IamExecutorRole::ALL_IN_ONE, IamResultCode::GENERAL_ERROR } },
        { static_cast<ExecutorRole>(ExecutorRole::ALL_IN_ONE + 1),
            { IamExecutorRole::ALL_IN_ONE, IamResultCode::GENERAL_ERROR } },
    };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_)).Times(Exactly(1)).WillOnce([&pair](ExecutorInfo &info) {
            info = {
                .executorRole = pair.first,
                .authType = AuthType::FACE,
                .esl = ExecutorSecureLevel::ESL0,
            };
            return HDF_SUCCESS;
        });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi->GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.executorRole == pair.second.first);
        }
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetExecutorInfo_005, TestSize.Level0)
{
    static const std::map<ExecutorSecureLevel, pair<IamExecutorSecureLevel, IamResultCode>> data = {
        { ExecutorSecureLevel::ESL0, { IamExecutorSecureLevel::ESL0, IamResultCode::SUCCESS } },
        { ExecutorSecureLevel::ESL1, { IamExecutorSecureLevel::ESL1, IamResultCode::SUCCESS } },
        { ExecutorSecureLevel::ESL2, { IamExecutorSecureLevel::ESL2, IamResultCode::SUCCESS } },
        { ExecutorSecureLevel::ESL3, { IamExecutorSecureLevel::ESL3, IamResultCode::SUCCESS } },
        { static_cast<ExecutorSecureLevel>(ExecutorSecureLevel::ESL0 - 1),
            { IamExecutorSecureLevel::ESL3, IamResultCode::GENERAL_ERROR } },
        { static_cast<ExecutorSecureLevel>(ExecutorSecureLevel::ESL3 + 1),
            { IamExecutorSecureLevel::ESL3, IamResultCode::GENERAL_ERROR } },
    };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_)).Times(Exactly(1)).WillOnce([&pair](ExecutorInfo &info) {
            info = {
                .executorRole = ExecutorRole::ALL_IN_ONE,
                .authType = AuthType::FACE,
                .esl = pair.first,
            };
            return HDF_SUCCESS;
        });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi->GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.esl == pair.second.first);
        }
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetExecutorInfo_006, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    IamExecutorInfo info = {};
    auto ret = executorHdi->GetExecutorInfo(info);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_OnRegisterFinish_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, RegisterSaCommandCallback(_))
            .WillRepeatedly([](const sptr<ISaCommandCallback> &callbackObj) { return HDF_SUCCESS; });

        EXPECT_CALL(*executorProxy, OnRegisterFinish(_, _, _))
            .Times(Exactly(1))
            .WillOnce(
                [&pair](const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &frameworkPublicKey,
                    const std::vector<uint8_t> &extraInfo) { return pair.first; });
        if (pair.first == HDF_SUCCESS) {
            EXPECT_CALL(*executorProxy, RegisterSaCommandCallback(_))
                .Times(Exactly(1))
                .WillOnce([](const sptr<ISaCommandCallback> &callbackObj) { return HDF_SUCCESS; });
        }
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto ret =
            executorHdi->OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_OnRegisterFinish_002, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    auto ret = executorHdi->OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_OnRegisterFinish_003, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, OnRegisterFinish(_, _, _))
            .Times(Exactly(1))
            .WillOnce([](const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &frameworkPublicKey,
                          const std::vector<uint8_t> &extraInfo) { return HDF_SUCCESS; });
        EXPECT_CALL(*executorProxy, RegisterSaCommandCallback(_))
            .Times(Exactly(1))
            .WillOnce([&pair](const sptr<ISaCommandCallback> &callbackObj) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto ret =
            executorHdi->OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Enroll_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Enroll(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
                          const sptr<IExecutorCallback> &callbackObj) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi->Enroll(0, UserAuth::EnrollParam { 0, std::vector<uint8_t>() }, executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Enroll_002, TestSize.Level0)
{
    sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Enroll(_, _, _)).Times(Exactly(0));
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
    auto ret = executorHdi->Enroll(0, UserAuth::EnrollParam { 0, std::vector<uint8_t>() }, nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Enroll_003, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi->Enroll(0, UserAuth::EnrollParam { 0, std::vector<uint8_t>() }, executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Authenticate_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Authenticate(_, _, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint64_t> &templateIdList,
                          const std::vector<uint8_t> &extraInfo,
                          const sptr<IExecutorCallback> &callbackObj) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi->Authenticate(0,
            UserAuth::AuthenticateParam { 0, std::vector<uint64_t>(), std::vector<uint8_t>(), false }, executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Authenticate_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Authenticate(_, _, _, _)).Times(Exactly(0));
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
    auto ret = executorHdi->Authenticate(0,
        UserAuth::AuthenticateParam { 0, std::vector<uint64_t>(), std::vector<uint8_t>(), false }, nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Authenticate_003, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi->Authenticate(0,
        UserAuth::AuthenticateParam { 0, std::vector<uint64_t>(), std::vector<uint8_t>(), false }, executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}
HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Identify_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Identify(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
                          const sptr<IExecutorCallback> &callbackObj) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi->Identify(0, UserAuth::IdentifyParam { 0, std::vector<uint8_t>() }, executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Identify_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Identify(_, _, _)).Times(Exactly(0));
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
    auto ret = executorHdi->Identify(0, UserAuth::IdentifyParam { 0, std::vector<uint8_t>() }, nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Identify_003, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi->Identify(0, UserAuth::IdentifyParam { 0, std::vector<uint8_t>() }, executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Delete_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Delete(_))
            .Times(Exactly(1))
            .WillOnce([&pair](const std::vector<uint64_t> &templateIdList) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto ret = executorHdi->Delete(std::vector<uint64_t>());
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Delete_002, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    auto ret = executorHdi->Delete(std::vector<uint64_t>());
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Cancel_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Cancel(_)).Times(Exactly(1)).WillOnce([&pair](uint64_t scheduleId) {
            return pair.first;
        });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto ret = executorHdi->Cancel(0);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_Cancel_002, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    auto ret = executorHdi->Cancel(0);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_SendCommand_001, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi->SendCommand(IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_SendCommand_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, SendCommand(_, _, _)).Times(Exactly(0));
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);

    auto ret = executorHdi->SendCommand(IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_SendCommand_003, TestSize.Level0)
{
    static const std::map<IamPropertyMode, pair<DriverCommandId, IamResultCode>> data = {
        { IamPropertyMode::PROPERTY_INIT_ALGORITHM, { DriverCommandId::INIT_ALGORITHM, IamResultCode::SUCCESS } },
        { IamPropertyMode::PROPERTY_MODE_FREEZE, { DriverCommandId::LOCK_TEMPLATE, IamResultCode::SUCCESS } },
        { IamPropertyMode::PROPERTY_MODE_UNFREEZE, { DriverCommandId::UNLOCK_TEMPLATE, IamResultCode::SUCCESS } },
        { static_cast<IamPropertyMode>(IamPropertyMode::PROPERTY_MODE_FREEZE - 1),
            { DriverCommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS } },
        { static_cast<IamPropertyMode>(IamPropertyMode::PROPERTY_MODE_UNFREEZE + 1),
            { DriverCommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS } },
        { static_cast<IamPropertyMode>(DriverCommandId::VENDOR_COMMAND_BEGIN),
            { DriverCommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS } },
        { static_cast<IamPropertyMode>(DriverCommandId::VENDOR_COMMAND_BEGIN + 1),
            { static_cast<DriverCommandId>(DriverCommandId::VENDOR_COMMAND_BEGIN + 1), IamResultCode::SUCCESS } }
    };
    for (const auto &pair : data) {
        sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
        ASSERT_TRUE(executorProxy != nullptr);
        if (pair.second.second == IamResultCode::SUCCESS) {
            EXPECT_CALL(*executorProxy, SendCommand(_, _, _))
                .Times(Exactly(1))
                .WillOnce([&pair](int32_t commandId, const std::vector<uint8_t> &extraInfo,
                              const sptr<IExecutorCallback> &callbackObj) {
                    EXPECT_TRUE(commandId == pair.second.first);
                    return HDF_SUCCESS;
                });
        } else {
            EXPECT_CALL(*executorProxy, SendCommand(_, _, _)).Times(Exactly(0));
        }
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi->SendCommand(pair.first, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_SendCommand_004, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, SendCommand(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](int32_t commandId, const std::vector<uint8_t> &extraInfo,
                          const sptr<IExecutorCallback> &callbackObj) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        auto executeCallback = MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret =
            executorHdi->SendCommand(IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetProperty_001, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    ASSERT_TRUE(executorHdi != nullptr);
    std::vector<uint64_t> templateIdList;
    std::vector<UserAuth::Attributes::AttributeKey> keys;
    UserAuth::Property property = {};
    auto ret = executorHdi->GetProperty(templateIdList, keys, property);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetProperty_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
    ASSERT_TRUE(executorHdi != nullptr);
    std::vector<uint64_t> templateIdList;
    std::vector<UserAuth::Attributes::AttributeKey> keys = { UserAuth::Attributes::ATTR_SIGNATURE };
    UserAuth::Property property = {};
    auto ret = executorHdi->GetProperty(templateIdList, keys, property);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_GetProperty_003, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetProperty(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](const std::vector<uint64_t> &templateIdList, const std::vector<int32_t> &propertyTypes,
                          Property &property) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        ASSERT_TRUE(executorHdi != nullptr);
        std::vector<uint64_t> templateIdList;
        std::vector<UserAuth::Attributes::AttributeKey> keys;
        if (pair.first != HDF_SUCCESS) {
            keys.push_back(UserAuth::Attributes::ATTR_PIN_SUB_TYPE);
        }
        UserAuth::Property property = {};
        auto ret = executorHdi->GetProperty(templateIdList, keys, property);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_SetCachedTemplates_001, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    ASSERT_TRUE(executorHdi != nullptr);
    std::vector<uint64_t> templateIdList;
    auto ret = executorHdi->SetCachedTemplates(templateIdList);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_SetCachedTemplates_002, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) MockIAllInOneExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, SetCachedTemplates(_))
            .Times(Exactly(1))
            .WillOnce([&pair](const std::vector<uint64_t> &templateIdList) { return pair.first; });
        auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
        ASSERT_TRUE(executorHdi != nullptr);
        std::vector<uint64_t> templateIdList;
        auto ret = executorHdi->SetCachedTemplates(templateIdList);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_OnHdiDisconnect_001, TestSize.Level0)
{
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(nullptr);
    ASSERT_TRUE(executorHdi != nullptr);
    executorHdi->OnHdiDisconnect();
}

HWTEST_F(FaceAuthAllInOneExecutorHdiUnitTest, FaceAuthAllInOneExecutorHdi_SaCommandCallback_OnSaCommands_001,
    TestSize.Level0)
{
    sptr<MockIAllInOneExecutor> executorProxy(new (std::nothrow) MockIAllInOneExecutor());
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, OnRegisterFinish(_, _, _))
        .Times(Exactly(1))
        .WillOnce([](const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &frameworkPublicKey,
                      const std::vector<uint8_t> &extraInfo) { return HDF_SUCCESS; });
    EXPECT_CALL(*executorProxy, RegisterSaCommandCallback(_))
        .Times(Exactly(1))
        .WillOnce([](const sptr<ISaCommandCallback> &callbackObj) {
            std::vector<SaCommand> commands;
            callbackObj->OnSaCommands(commands);
            return HDF_SUCCESS;
        });
    auto executorHdi = MakeShared<FaceAuthAllInOneExecutorHdi>(executorProxy);
    auto ret = executorHdi->OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
    EXPECT_TRUE(ret == IamResultCode::SUCCESS);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
