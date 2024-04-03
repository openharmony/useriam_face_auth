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
#include "face_auth_driver_hdi.h"
#include "mock_face_auth_interface_adapter.h"
#include "mock_iall_in_one_executor.h"
#include "mock_iface_auth_interface.h"

#define LOG_TAG "FACE_AUTH_SA"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::UserIam::Common;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthDriverHdiUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FaceAuthDriverHdiUnitTest::SetUpTestCase()
{
}

void FaceAuthDriverHdiUnitTest::TearDownTestCase()
{
}

void FaceAuthDriverHdiUnitTest::SetUp()
{
}

void FaceAuthDriverHdiUnitTest::TearDown()
{
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_001, TestSize.Level0)
{
    FaceAuthDriverHdi driverHdi(nullptr);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_002, TestSize.Level0)
{
    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(nullptr));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_003, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_)).Times(Exactly(1));

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_004, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) { return static_cast<int32_t>(HDF_FAILURE); });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_005, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) { return static_cast<int32_t>(HDF_SUCCESS); });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_006, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_SUCCESS);
        });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 1);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_007, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            list.push_back(sptr<IAllInOneExecutor>(nullptr));
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            list.push_back(sptr<IAllInOneExecutor>(nullptr));
            executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_SUCCESS);
        });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 2);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_GetExecutorListTest_008, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_FAILURE);
        });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_OnHdiDisconnectTest_001, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_SUCCESS);
        });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 1);
    driverHdi.OnHdiDisconnect();
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_SetBufferProducer_001, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, SetBufferProducer(_))
        .Times(Exactly(1))
        .WillOnce([](const sptr<BufferProducerSequenceable> &producer) { return static_cast<int32_t>(HDF_SUCCESS); });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    sptr<IBufferProducer> producer = nullptr;
    int32_t ret = driverHdi.SetBufferProducer(producer);
    EXPECT_EQ(ret, FACE_AUTH_SUCCESS);
}

HWTEST_F(FaceAuthDriverHdiUnitTest, FaceAuthDriverHdi_SetBufferProducer_002, TestSize.Level0)
{
    sptr<MockIFaceAuthInterface> interface(new (std::nothrow) MockIFaceAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, SetBufferProducer(_))
        .Times(Exactly(1))
        .WillOnce(
            [](const sptr<BufferProducerSequenceable> &producer) { return static_cast<int32_t>(FACE_AUTH_ERROR); });

    auto adapter = MakeShared<MockFaceAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FaceAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    sptr<IBufferProducer> producer = nullptr;
    int32_t ret = driverHdi.SetBufferProducer(producer);
    EXPECT_EQ(ret, FACE_AUTH_ERROR);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
