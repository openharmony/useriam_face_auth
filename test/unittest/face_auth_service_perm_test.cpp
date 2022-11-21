/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "message_parcel.h"

#include "face_auth_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthServicePermTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FaceAuthServicePermTest::SetUpTestCase()
{
}

void FaceAuthServicePermTest::TearDownTestCase()
{
}

void FaceAuthServicePermTest::SetUp()
{
}

void FaceAuthServicePermTest::TearDown()
{
}

HWTEST_F(FaceAuthServicePermTest, FaceAuthServicePermTest_001, TestSize.Level0)
{
    auto service = FaceAuthService::GetInstance();
    EXPECT_NE(service, nullptr);
    sptr<IBufferProducer> producer = nullptr;
    int32_t ret = service->SetBufferProducer(producer);
    EXPECT_EQ(ret, FACE_AUTH_CHECK_PERMISSION_FAILED);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
