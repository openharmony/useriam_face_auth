/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "face_auth_manager_scenario_test.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include "useriam_auth_kit.h"
#include "mock_common.h"

#define private public

using namespace std;
using namespace testing::ext;
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const auto timeFast = std::chrono::milliseconds(100);
const auto timebusy = std::chrono::milliseconds(500);
const auto waitcamera = std::chrono::milliseconds(1500);
const std::int32_t FILE_PERMISSION_FLAG = 00766;

static int32_t SaveYUV(const char *buffer, int32_t size)
{
    char path[PATH_MAX] = {0};
    int32_t retlen = 0;
    retlen = sprintf_s(path, sizeof(path) / sizeof(path[0]), "/data/%s.yuv", "ST");
    if (retlen < 0) {
        FACEAUTH_LABEL_LOGI("Path Assignment failed");
        return -1;
    }

    FACEAUTH_LABEL_LOGI("%s, saving file to %{public}s", __FUNCTION__, path);
    int32_t imgFd = open(path, O_RDWR | O_CREAT, FILE_PERMISSION_FLAG);
    if (imgFd == -1) {
        FACEAUTH_LABEL_LOGI("%s, open file failed, errno = %{public}s.", __FUNCTION__, strerror(errno));
        return -1;
    }
    int32_t ret = write(imgFd, buffer, size);
    if (ret == -1) {
        FACEAUTH_LABEL_LOGI("%s, write file failed, error = %{public}s", __FUNCTION__, strerror(errno));
        close(imgFd);
        return -1;
    }
    close(imgFd);
    return 0;
}

void FaceAuthScenarioTest::FaceAuthCameraBufferListener::OnBufferAvailable()
{
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage;
    FACEAUTH_LABEL_LOGI("FaceAuthCameraBufferListener OnBufferAvailable");
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    cameraBuffer_->AcquireBuffer(buffer, flushFence, timestamp, damage);
    if (buffer != nullptr) {
        char *addr = static_cast<char *>(buffer->GetVirAddr());
        int32_t size = buffer->GetSize();
        FACEAUTH_LABEL_LOGI("Calling SaveYUV");
        SaveYUV(addr, size);
        cameraBuffer_->ReleaseBuffer(buffer, -1);
    } else {
        FACEAUTH_LABEL_LOGI("AcquireBuffer failed!");
    }
}

static sptr<Surface> CreatePreviewOutput4UI()
{
    FACEAUTH_LABEL_LOGI("CreatePreviewOutput4UI.");
    sptr<Surface> previewBuffer = Surface::CreateSurfaceAsConsumer();
    previewBuffer->SetDefaultWidthAndHeight(PREVIEW_DEFAULT_WIDTH, PREVIEW_DEFAULT_HEIGHT);
    previewBuffer->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                               std::to_string(OHOS_CAMERA_FORMAT_YCRCB_420_SP));
    sptr<FaceAuthScenarioTest::FaceAuthCameraBufferListener> listener =
        new FaceAuthScenarioTest::FaceAuthCameraBufferListener();
    listener->cameraBuffer_ = previewBuffer;
    previewBuffer->RegisterConsumerListener((sptr<IBufferConsumerListener> &) listener);
    return previewBuffer;
}

void FaceAuthScenarioTest::WriteFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_)
{
    std::ofstream mystream("/data/config.dat", ios::trunc);
    int32_t paramNum = 10;
    if (!mystream.is_open()) {
        return;
    }
    mystream << (to_string(caseNum) + "\n");
    mystream << (to_string(codeNum) + "\n");
    for (size_t i = 0; i < resultInfos_.size(); i++) {
        mystream << to_string(resultInfos_[i].resultCode);
        mystream << " ";
        for (int32_t j = 0; j < paramNum; j++) {
            mystream << to_string(resultInfos_[i].param[j]);
            mystream << " ";
        }
        mystream << "\n";
    }
    mystream.close();
}

/**
 * @tc.number: CPPAPI_Function_Scenario_0100
 * @tc.name:: Requests are sorted by priority
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthScenarioTest, CPPAPI_Function_Scenario_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthScenarioTest CPPAPI_Function_Scenario_0100 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 3800;
    uint64_t reqIdRemoteNum = 3803;
    uint64_t reqIdLocalNum = 3802;
    uint64_t reqIdEnrollNum = 3801;
    uint64_t reqIdRemoveNum = 3804;
    param.reqId = reqIdNum;
    param.flags = 1;
    param.challenge = 1;
    param.faceId = 0;
    service->Authenticate(param, callback);
    std::this_thread::sleep_for(timeFast);
    OHOS::sptr<TestCallback> remoteCallback(new TestCallback());
    param.reqId = reqIdRemoteNum;
    service->Authenticate(param, remoteCallback);
    param.flags = 0;
    OHOS::sptr<TestCallback> localCallback(new TestCallback());
    param.reqId = reqIdLocalNum;
    service->Authenticate(param, localCallback);
    OHOS::sptr<TestCallback> enrollCallback(new TestCallback());
    EnrollParam enrollParam;
    enrollParam.challenge = 1;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        enrollParam.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    enrollParam.producer = cameraBuffer->GetProducer();
    enrollParam.faceId = 0;
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        enrollParam.token.push_back(*iter);
    }
    enrollParam.reqId = reqIdEnrollNum;
    service->Enroll(enrollParam, enrollCallback);
    OHOS::sptr<TestCallback> removeCallback(new TestCallback());
    RemoveParam removeParam;
    removeParam.faceId = 0;
    removeParam.reqId = reqIdRemoveNum;
    service->Remove(removeParam, removeCallback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    CPPAPI_Function_Scenario_0100_Check(remoteCallback, localCallback, enrollCallback, removeCallback);
    GTEST_LOG_(INFO) << "FaceAuthScenarioTest CPPAPI_Function_Scenario_0100 end";
}

void FaceAuthScenarioTest::CPPAPI_Function_Scenario_0100_Check(OHOS::sptr<TestCallback> remoteCallback,
    OHOS::sptr<TestCallback> localCallback, OHOS::sptr<TestCallback> enrollCallback,
    OHOS::sptr<TestCallback> removeCallback)
{
    while (!remoteCallback->isCallback_ && !removeCallback->isCallback_ && !enrollCallback->isCallback_ &&
        !localCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(removeCallback->isCallback_, true);
    GTEST_LOG_(INFO) << "remove success";
    while (!remoteCallback->isCallback_ && !enrollCallback->isCallback_ && !localCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(enrollCallback->isCallback_, true);
    GTEST_LOG_(INFO) << "enroll success";
    while (!remoteCallback->isCallback_ && !localCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(localCallback->isCallback_, true);
    GTEST_LOG_(INFO) << "local success";
    while (!remoteCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(remoteCallback->isCallback_, true);
    GTEST_LOG_(INFO) << "remote success";
    std::this_thread::sleep_for(waitcamera);
}

/**
 * @tc.number: CPPAPI_Function_Scenario_0200
 * @tc.name:: In the same priority request order
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthScenarioTest, CPPAPI_Function_Scenario_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthScenarioTest CPPAPI_Function_Scenario_0200 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    uint64_t reqIdNum = 3900;
    uint64_t reqIdRemoteNum = 3901;
    uint64_t reqIdRemoteNum1 = 3902;
    uint64_t reqIdRemoteNum2 = 3903;
    AuthParam param;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    service->Authenticate(param, callback);
    std::this_thread::sleep_for(timebusy);
    OHOS::sptr<TestCallback> remotecallback1(new TestCallback());
    param.reqId = reqIdRemoteNum;
    param.flags = 1;
    service->Authenticate(param, remotecallback1);
    OHOS::sptr<TestCallback> remotecallback2(new TestCallback());
    param.reqId = reqIdRemoteNum1;
    service->Authenticate(param, remotecallback2);
    OHOS::sptr<TestCallback> remotecallback3(new TestCallback());
    param.reqId = reqIdRemoteNum2;
    service->Authenticate(param, remotecallback3);

    while (!remotecallback1->isCallback_ && !remotecallback2->isCallback_ && !remotecallback3->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(remotecallback1->isCallback_, true);
    GTEST_LOG_(INFO) << "remotecallback1 success";
    while (!remotecallback2->isCallback_ && !remotecallback3->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(remotecallback2->isCallback_, true);
    GTEST_LOG_(INFO) << "remotecallback2 success";
    while (!remotecallback3->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(remotecallback3->isCallback_, true);
    GTEST_LOG_(INFO) << "remotecallback3 success";
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << "FaceAuthScenarioTest CPPAPI_Function_Scenario_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_Scenario_0300
 * @tc.name:: After the authentication is successful, the authentication fails for five times
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthScenarioTest, CPPAPI_Function_Scenario_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Scenario_0300 start";
    service->ResetTimeout();
    std::map<int32_t, ResultInfo> resultInfos_;
    ResultInfo info;
    int32_t resultcode = 18;
    info.resultCode = resultcode;
    int32_t range = 10;
    for (int32_t j = 0; j < range; j++) {
        info.param[j] = 0;
    }
    resultInfos_[0] = info;
    int32_t numFour = 4;
    int32_t numThree = 3;
    CPPAPI_Function_Scenario_0300_Auth(numFour, resultInfos_);
    CPPAPI_Function_Scenario_0300_Auth(numThree, resultInfos_);
    OHOS::sptr<TestCallback> callbackAuth(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 4400;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    service->Authenticate(param, callbackAuth);
    while (!callbackAuth->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    int32_t result = service->GetRemainingNum();
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    int32_t num = 5;
    EXPECT_EQ(result, num);
    service->ResetTimeout();
    std::this_thread::sleep_for(timebusy);
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Scenario_0300 end";
}

void FaceAuthScenarioTest::CPPAPI_Function_Scenario_0300_Auth(int32_t num, std::map<int32_t, ResultInfo> resultInfos_)
{
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 4400;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    WriteFile(1, 1, resultInfos_);
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    std::this_thread::sleep_for(waitcamera);
    result = service->GetRemainingNum();
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    EXPECT_EQ(result, num);
}

/**
 * @tc.number: CPPAPI_Function_Scenario_0400
 * @tc.name:: Cancels authentication and entry in the queue
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthScenarioTest, CPPAPI_Function_Scenario_0400, Function | MediumTest | Level1)
{
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param = CPPAPI_Function_Scenario_0400_LocalAuth();
    service->Authenticate(param, callback);
    std::this_thread::sleep_for(timeFast);
    OHOS::sptr<TestCallback> remoteCallback(new TestCallback());
    param = CPPAPI_Function_Scenario_0400_LocalAnotherAuth();
    service->Authenticate(param, remoteCallback);
    param = CPPAPI_Function_Scenario_0400_CoAuth();
    OHOS::sptr<TestCallback> localCallback(new TestCallback());
    service->Authenticate(param, localCallback);
    EnrollParam enrollParam = CPPAPI_Function_Scenario_0400_Enroll();
    OHOS::sptr<TestCallback> enrollCallback(new TestCallback());
    service->Enroll(enrollParam, enrollCallback);
    RemoveParam removeParam = CPPAPI_Function_Scenario_0400_Remove();
    OHOS::sptr<TestCallback> removeCallback(new TestCallback());
    service->Remove(removeParam, removeCallback);
    OHOS::sptr<TestCallback> callbackCancelAuth(new TestCallback());
    service->CancelAuth(param.reqId, callbackCancelAuth);
    while (!callbackCancelAuth->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    OHOS::sptr<TestCallback> callbackCancelEnroll(new TestCallback());
    service->CancelEnrollment(enrollParam.reqId, callbackCancelEnroll);
    while (!callbackCancelEnroll->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    CPPAPI_Function_Scenario_0100_Check(remoteCallback, localCallback, enrollCallback, removeCallback);
    GTEST_LOG_(INFO) << "FaceAuthScenarioTest CPPAPI_Function_Scenario_0100 end";
}

AuthParam FaceAuthScenarioTest::CPPAPI_Function_Scenario_0400_LocalAuth()
{
    AuthParam param;
    uint64_t reqIdNum = 4300;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    return param;
}

AuthParam FaceAuthScenarioTest::CPPAPI_Function_Scenario_0400_LocalAnotherAuth()
{
    AuthParam param;
    uint64_t reqIdNum = 4303;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    return param;
}

AuthParam FaceAuthScenarioTest::CPPAPI_Function_Scenario_0400_CoAuth()
{
    AuthParam param;
    uint64_t reqIdCoNum = 4302;
    param.reqId = reqIdCoNum;
    param.flags = 1;
    param.challenge = 1;
    param.faceId = 0;
    return param;
}

EnrollParam FaceAuthScenarioTest::CPPAPI_Function_Scenario_0400_Enroll()
{
    EnrollParam enrollParam;
    uint64_t reqIdEnrollNum = 4301;
    enrollParam.challenge = 1;
    enrollParam.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        enrollParam.token.push_back(*iter);
    }
    enrollParam.reqId = reqIdEnrollNum;
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    enrollParam.producer = cameraBuffer->GetProducer();
    return enrollParam;
}

RemoveParam FaceAuthScenarioTest::CPPAPI_Function_Scenario_0400_Remove()
{
    RemoveParam removeParam;
    uint64_t reqIdRemoveNum = 4304;
    removeParam.faceId = 0;
    removeParam.reqId = reqIdRemoveNum;
    return removeParam;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS