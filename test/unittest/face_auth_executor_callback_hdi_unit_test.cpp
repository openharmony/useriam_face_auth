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

#include "face_auth_defines.h"
#include "face_auth_executor_callback_hdi.h"
#include "mock_iexecute_callback.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

using namespace testing;
using namespace testing::ext;
using namespace OHOS::HDI::FaceAuth::V1_0;
using namespace OHOS::UserIam::UserAuth;
using namespace OHOS::UserIAM::Common;

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
using IamResultCode = OHOS::UserIam::UserAuth::ResultCode;
class FaceAuthExecutorCallbackHdiUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FaceAuthExecutorCallbackHdiUnitTest ::SetUpTestCase()
{
}

void FaceAuthExecutorCallbackHdiUnitTest ::TearDownTestCase()
{
}

void FaceAuthExecutorCallbackHdiUnitTest ::SetUp()
{
}

void FaceAuthExecutorCallbackHdiUnitTest ::TearDown()
{
}

HWTEST_F(FaceAuthExecutorCallbackHdiUnitTest, FaceAuthExecutorCallback_OnAcquireInfo_001, TestSize.Level0)
{
    auto executeCallback = MakeShared<UserIAM::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    const int32_t testAcquire = 5;
    const std::vector<uint8_t> testExtraInfo = {1, 2, 3, 4, 5, 6};
    EXPECT_CALL(*executeCallback, OnAcquireInfo(_, _))
        .Times(Exactly(1))
        .WillOnce([&testAcquire, &testExtraInfo](int32_t acquire, const std::vector<uint8_t> &extraInfo) {
            EXPECT_TRUE(acquire == testAcquire);
            EXPECT_TRUE(testExtraInfo.size() == extraInfo.size());
            EXPECT_TRUE(std::equal(extraInfo.begin(), extraInfo.end(), testExtraInfo.begin()));
        });
    FaceAuthExecutorCallbackHdi callbackHdi(executeCallback);
    callbackHdi.OnAcquireInfo(testAcquire, testExtraInfo);
}

HWTEST_F(FaceAuthExecutorCallbackHdiUnitTest, FaceAuthExecutorCallback_OnResult_001, TestSize.Level0)
{
    static const std::map<ResultCode, IamResultCode> data = {{ResultCode::SUCCESS, IamResultCode::SUCCESS},
        {ResultCode::FAIL, IamResultCode::FAIL}, {ResultCode::GENERAL_ERROR, IamResultCode::GENERAL_ERROR},
        {ResultCode::CANCELED, IamResultCode::CANCELED}, {ResultCode::TIMEOUT, IamResultCode::TIMEOUT},
        {ResultCode::BUSY, IamResultCode::BUSY},
        {ResultCode::INVALID_PARAMETERS, IamResultCode::INVALID_PARAMETERS},
        {ResultCode::LOCKED, IamResultCode::LOCKED},
        {ResultCode::NOT_ENROLLED, IamResultCode::NOT_ENROLLED},
        {ResultCode::OPERATION_NOT_SUPPORT, IamResultCode::FAIL},
        {static_cast<ResultCode>(ResultCode::VENDOR_RESULT_CODE_BEGIN - 1), IamResultCode::GENERAL_ERROR},
        {static_cast<ResultCode>(ResultCode::VENDOR_RESULT_CODE_BEGIN), IamResultCode::GENERAL_ERROR},
        {static_cast<ResultCode>(ResultCode::VENDOR_RESULT_CODE_BEGIN + 1),
            static_cast<IamResultCode>(ResultCode::VENDOR_RESULT_CODE_BEGIN + 1)}};

    for (const auto &pair : data) {
        auto executeCallback = MakeShared<UserIAM::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        std::vector<uint8_t> testExtraInfo = {1, 2, 3, 4, 5, 6};
        EXPECT_CALL(*executeCallback, OnResult(_, _))
            .Times(Exactly(1))
            .WillOnce([&pair, &testExtraInfo](int32_t result, const std::vector<uint8_t> &extraInfo) {
                EXPECT_TRUE(result == pair.second);
                EXPECT_TRUE(testExtraInfo.size() == extraInfo.size());
                EXPECT_TRUE(std::equal(extraInfo.begin(), extraInfo.end(), testExtraInfo.begin()));
            });
        FaceAuthExecutorCallbackHdi callbackHdi(executeCallback);
        callbackHdi.OnResult(pair.first, testExtraInfo);
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
