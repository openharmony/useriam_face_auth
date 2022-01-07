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

#include "face_auth_manager_enroll_test.h"
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
const auto waitcamera = std::chrono::milliseconds(1500);
static const int32_t ENROLL_SUCCESS = 998;
const std::int32_t FILE_PERMISSION_FLAG = 00766;
const int32_t PERFORMANCE_TEST_TIMES = 1000;
static int32_t FACEID = 1;

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

void FaceEnrollManagerTest::FaceAuthCameraBufferListener::OnBufferAvailable()
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
    sptr<FaceEnrollManagerTest::FaceAuthCameraBufferListener> listener =
        new FaceEnrollManagerTest::FaceAuthCameraBufferListener();
    listener->cameraBuffer_ = previewBuffer;
    previewBuffer->RegisterConsumerListener((sptr<IBufferConsumerListener> &) listener);
    return previewBuffer;
}

void FaceEnrollManagerTest::WriteFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_)
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

void FaceEnrollManagerTest::WritePreFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_)
{
    std::ofstream mystream("/data/pre.dat", ios::trunc);
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

void FaceEnrollManagerTest::WritePostFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_)
{
    std::ofstream mystream("/data/post.dat", ios::trunc);
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

void FaceEnrollManagerTest::CreateEnroll()
{
    std::ofstream mystream(FACEID_FILENAME, std::ios::trunc);
    if (mystream.is_open()) {
        mystream << (std::to_string(FACEID) + "\n");
        FACEID++;
    } else {
        mystream << (std::to_string(1) + "\n");
    }
    mystream.close();
}

/**
 * @tc.number: CPPAPI_Performance_Enroll_0100
 * @tc.name:: Performance of Enroll interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Performance_Enroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_Enroll_0100 start";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 100;
    param.reqId = reqIdNum;
    param.challenge = -1;
    param.faceId = -1;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        param.reqId++;
        callback->isCallback_ = false;
        auto startTime = std::chrono::high_resolution_clock::now();
        service->Enroll(param, callback);
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_Enroll_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_Enroll_0100 end";
}

/**
 * @tc.number: CPPAPI_Performance_CancelEnroll_0100
 * @tc.name:: Performance of CancelEnroll interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Performance_CancelEnroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_CancelEnroll_0100 start";

    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 1500;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
        auto startTime = std::chrono::high_resolution_clock::now();
        service->CancelEnrollment(param.reqId, callbackCancel);
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "FaceAuth_CPPAPI_1500  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_CancelEnroll_0100 end";
}

/**
 * @tc.number: CPPAPI_Performance_PreEnroll_0100
 * @tc.name:: Performance of PreEnroll interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Performance_PreEnroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_PreEnroll_0100";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->PreEnroll();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_PreEnroll_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_PreEnroll_0100 end";
}

/**
 * @tc.number: CPPAPI_Performance_PostEnroll_0100
 * @tc.name:: Performance of PostEnroll interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Performance_PostEnroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_PostEnroll_0100";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->PostEnroll();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_PostEnroll_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: CPPAPI_Performance_GetAngleDim_0100
 * @tc.name:: Performance of GetAngleDim interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Performance_GetAngleDim_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_GetAngleDim_0100";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->GetAngleDim();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
        averageTime = totalTime / (run_times + 1);
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_GetAngleDim_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    GTEST_LOG_(INFO) << result;
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: CPPAPI_Performance_GetEnrolledFaceIDs_0100
 * @tc.name:: Performance of GetEnrolledFaceIDs interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Performance_GetEnrolledFaceIDs_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_GetEnrolledFaceIDs_0100";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->GetEnrolledFaceIDs(0);
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_GetEnrolledFaceIDs_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: CPPAPI_Performance_Remove_0100
 * @tc.name:: Performance of Remove interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Performance_Remove_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest::CPPAPI_Performance_Remove_0100";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    OHOS::sptr<TestCallback> callback(new TestCallback());
    RemoveParam param;
    uint64_t reqIdNum = 100;
    param.reqId = reqIdNum;
    param.faceId = -1;
    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->Remove(param, callback);
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_Remove_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    GTEST_LOG_(INFO) << result;
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: CPPAPI_Function_CancelEnroll_0100
 * @tc.name:: Cancel the current entry with the same reqid as the enrolled
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_CancelEnroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0100 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 200;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(ENROLL_SUCCESS, 0, resultInfos_);
    service->Enroll(param, callback);
    std::this_thread::sleep_for(timeFast);
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    int32_t result = service->CancelEnrollment(param.reqId, callbackCancel);
    while (!callbackCancel->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(callback->errorcode_, ERRCODE_CANCEL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelEnroll_0200
 * @tc.name:: Cancel the current entry by using a reqid different from the enrolled
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_CancelEnroll_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0200 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 300;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(ENROLL_SUCCESS, 0, resultInfos_);
    service->Enroll(param, callback);
    std::this_thread::sleep_for(timeFast);
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    int num = 30;
    int32_t result = service->CancelEnrollment(num, callback);
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    EXPECT_EQ(result, -1);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", result);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelEnroll_0300
 * @tc.name:: Cancel callback is nullptr
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_CancelEnroll_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0300 start";
    int num = 30;
    int32_t result = service->CancelEnrollment(num, nullptr);
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    EXPECT_EQ(result, -1);
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", result);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0300 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelEnroll_0400
 * @tc.name:: Cancelauth is not called for enroll
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_CancelEnroll_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0400 start";
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    int num = 30;
    int32_t result = service->CancelEnrollment(num, callbackCancel);
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    EXPECT_EQ(result, -1);
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", result);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0400 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelEnroll_0500
 * @tc.name:: Call cancelauth repeatedly
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_CancelEnroll_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0500 start";
    EnrollParam param;
    uint64_t reqIdNum = 500;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(ENROLL_SUCCESS, 0, resultInfos_);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    service->Enroll(param, callback);
    std::this_thread::sleep_for(timeFast);
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    int32_t result = service->CancelEnrollment(reqIdNum, callbackCancel);
    if (result == 0) {
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
        EXPECT_EQ(callback->errorcode_, ERRCODE_CANCEL);
    }
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    OHOS::sptr<TestCallback> callbackCancel1(new TestCallback());
    result = service->CancelEnrollment(reqIdNum, callbackCancel1);
    EXPECT_EQ(result, -1);
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", result);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_CancelEnroll_0500 end";
}

/**
 * @tc.number: CPPAPI_Function_PreEnroll_0100
 * @tc.name:: Generate a challenge using preenroll
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_PreEnroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PreEnroll_0100 start";
    int64_t challeng = service->PreEnroll();
    int32_t result = -1;
    if (challeng > 0) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PreEnroll_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_PreEnroll_0200
 * @tc.name:: Generate a challenge using preenroll Fail
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_PreEnroll_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PreEnroll_0200 start";
    std::map<int32_t, ResultInfo> resultInfos_;
    WritePreFile(FACE_AUTH_FAIL, 0, resultInfos_);
    int32_t result = service->PreEnroll();
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PreEnroll_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_PostEnroll_0100
 * @tc.name:: End the face enroll and remove the challenge generated at the bottom
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_PostEnroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PostEnroll_0100 start";
    int32_t result = service->PostEnroll();
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PostEnroll_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_PostEnroll_0200
 * @tc.name:: End the face enroll and remove the challenge generated at the bottom Fail
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_PostEnroll_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PostEnroll_0200 start";
    std::map<int32_t, ResultInfo> resultInfos_;
    WritePostFile(FACE_AUTH_FAIL, 0, resultInfos_);
    int32_t result = service->PostEnroll();
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_PostEnroll_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_GetAngleDim_0100
 * @tc.name:: Get the number of face angles to be entered
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_GetAngleDim_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetAngleDim_0100 start";
    int32_t result = service->GetAngleDim();
    EXPECT_EQ(result, 3);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetAngleDim_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_GetEnrolledFaceIDs_0100
 * @tc.name:: GetEnrolledFaceIDs is called when userid = - 1
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_GetEnrolledFaceIDs_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetEnrolledFaceIDs_0100 start";
    std::list<int32_t> result = service->GetEnrolledFaceIDs(-1);
    EXPECT_EQ(result.size(), 0);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetEnrolledFaceIDs_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_GetEnrolledFaceIDs_0200
 * @tc.name:: GetEnrolledFaceIDs is called when userid = 0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_GetEnrolledFaceIDs_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetEnrolledFaceIDs_0200 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    RemoveParam param;
    uint64_t reqIdNum = 2800;
    param.reqId = reqIdNum;
    param.faceId = 0;
    service->Remove(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    std::list<int32_t> result = service->GetEnrolledFaceIDs(0);
    EXPECT_EQ(result.size(), 0);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetEnrolledFaceIDs_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_GetEnrolledFaceIDs_0300
 * @tc.name:: GetEnrolledFaceIDs is called when userid is greater than 0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_GetEnrolledFaceIDs_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetEnrolledFaceIDs_0300 start";
    std::list<int32_t> result = service->GetEnrolledFaceIDs(1);
    EXPECT_EQ(result.size(), 0);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_GetEnrolledFaceIDs_0300 end";
}

/**
 * @tc.number: CPPAPI_Function_Remove_0100
 * @tc.name:: GetEnrolledFaceIDs is called when faceId = -1
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Remove_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0100 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    RemoveParam param;
    uint64_t reqIdNum = 3300;
    param.reqId = reqIdNum;
    param.faceId = -1;
    int32_t result = service->Remove(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_FAIL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_Remove_0200
 * @tc.name:: Call remove to clear all face templates
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Remove_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0200 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    RemoveParam param;
    uint64_t reqIdNum = 3400;
    param.reqId = reqIdNum;
    param.faceId = 0;
    int32_t result = service->Remove(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_SUCCESS);
    std::list<int32_t> num = service->GetEnrolledFaceIDs(0);
    EXPECT_EQ(num.size(), 0);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_Remove_0300
 * @tc.name:: Call remove to delete the face template with the specified ID
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Remove_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0300 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    RemoveParam param;
    uint64_t reqIdNum = 3500;
    param.reqId = reqIdNum;
    param.faceId = 0;
    OHOS::sptr<TestCallback> removeCallback(new TestCallback());
    int32_t result = service->Remove(param, removeCallback);
    while (!removeCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(removeCallback->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0300 end";
}

/**
 * @tc.number: CPPAPI_Function_Remove_0400
 * @tc.name:: Call remove to delete the face template with the specified ID
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Remove_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0400 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    RemoveParam param;
    uint64_t reqIdNum = 3501;
    param.reqId = reqIdNum;
    param.faceId = 0;
    OHOS::sptr<TestCallback> removeCallback(new TestCallback());
    int32_t result = service->Remove(param, removeCallback);
    OHOS::sptr<TestCallback> removeCallback1(new TestCallback());
    result = service->Remove(param, removeCallback1);
    while (!removeCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    while (!removeCallback1->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(removeCallback1->errorcode_, ERRCODE_BUSY);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0400 end";
}

/**
 * @tc.number: CPPAPI_Function_Remove_0500
 * @tc.name:: Call remove to delete the face template with the specified ID
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Remove_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0500 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    RemoveParam param;
    uint64_t reqIdNum = 3501;
    param.reqId = reqIdNum;
    param.faceId = 0;
    OHOS::sptr<TestCallback> removeCallback(new TestCallback());
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(FACE_AUTH_GETRESULT_FAIL, 0, resultInfos_);
    int32_t result = service->Remove(param, removeCallback);
    while (!removeCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(removeCallback->errorcode_, ERRCODE_FAIL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0500 end";
}

/**
 * @tc.number: CPPAPI_Function_Remove_0600
 * @tc.name:: remove callback is nullptr
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Remove_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0600 start";
    RemoveParam param;
    uint64_t reqIdNum = 3501;
    param.reqId = reqIdNum;
    param.faceId = 0;
    OHOS::sptr<TestCallback> removeCallback(new TestCallback());
    int32_t result = service->Remove(param, nullptr);
    EXPECT_EQ(result, -1);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Remove_0600 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0100
 * @tc.name:: Face enroll when token size =0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0100 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 600;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    int32_t result = service->Enroll(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(callback->errorcode_, ERRCODE_INVALID_PARAMETERS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0200
 * @tc.name:: Face enroll when faceId = -1
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0200 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 900;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = -1;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    int32_t result = service->Enroll(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(callback->errorcode_, ERRCODE_INVALID_PARAMETERS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0300
 * @tc.name:: Face enroll when faceId = 0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0300 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 1000;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(ENROLL_SUCCESS, 0, resultInfos_);
    int32_t result = service->Enroll(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(callback->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0300 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0400
 * @tc.name:: Face enroll when entered faceid
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0400 start";
    EnrollParam param;
    uint64_t reqIdNum = 1100;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(ENROLL_SUCCESS, 0, resultInfos_);
    OHOS::sptr<TestCallback> enrollCallback(new TestCallback());
    int32_t result = service->Enroll(param, enrollCallback);
    while (!enrollCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("enrollCallback->errorcode_ is %{public}d", enrollCallback->errorcode_);
    EXPECT_EQ(enrollCallback->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0400 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0500
 * @tc.name:: Face enroll when faceid not entered
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0500 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 1200;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 10;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    CreateEnroll();
    OHOS::sptr<TestCallback> enrollCallback(new TestCallback());
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(ENROLL_SUCCESS, 0, resultInfos_);
    int32_t result = service->Enroll(param, enrollCallback);
    while (!enrollCallback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(enrollCallback->errorcode_, ERRCODE_NO_FACE_DATA);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0500 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0600
 * @tc.name:: Face enroll when callback is nullptr
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0600 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 1300;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(ENROLL_SUCCESS, 0, resultInfos_);
    int32_t result = service->Enroll(param, nullptr);
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(result, -1);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0600 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0800
 * @tc.name:: Face Enroll when busy
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0800 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    OHOS::sptr<TestCallback> callback1(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 900;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    int32_t result = service->Enroll(param, callback);
    result = service->Enroll(param, callback1);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    while (!callback1->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback1->errorcode_);
    EXPECT_EQ(callback1->errorcode_, ERRCODE_BUSY);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0800 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_0900
 * @tc.name:: Face Enroll when GetResult File
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0900 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 1000;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(FACE_AUTH_GETRESULT_FAIL, 0, resultInfos_);
    int32_t result = service->Enroll(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(callback->errorcode_, ERRCODE_FAIL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_0900 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_1000
 * @tc.name:: Face Enroll when GetResult TimeOut
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_1000 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 1100;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(FACE_AUTH_GETRESULT_TIMEOUT, 0, resultInfos_);
    int32_t result = service->Enroll(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(callback->errorcode_, ERRCODE_TIMEOUT);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_1000 end";
}

/**
 * @tc.number: CPPAPI_Function_Enroll_1100
 * @tc.name:: Face Enroll when carmera open fail
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceEnrollManagerTest, CPPAPI_Function_Enroll_1100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_1100 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    EnrollParam param;
    uint64_t reqIdNum = 1200;
    param.reqId = reqIdNum;
    param.challenge = 1;
    param.faceId = 0;
    std::vector<uint8_t> token;
    token.push_back('9');
    for (std::vector<uint8_t>::const_iterator iter = token.begin(); iter != token.end(); ++iter) {
        param.token.push_back(*iter);
    }
    sptr<Surface> cameraBuffer = CreatePreviewOutput4UI();
    param.producer = cameraBuffer->GetProducer();
    int32_t kill_times = 5;
    const auto temp_time = std::chrono::milliseconds(1000);
    while (kill_times) {
        FILE *ptr;
        ptr = popen("kill -9 $(pidof camera_service)", "r");
        pclose(ptr);
        kill_times--;
        std::this_thread::sleep_for(temp_time);
    }
    int32_t result = service->Enroll(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(callback->errorcode_, ERRCODE_CAMERA_FAIL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceEnrollManagerTest CPPAPI_Function_Enroll_1100 end";
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS