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

#include "face_auth_manager_auth_test.h"
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
const int32_t PERFORMANCE_TEST_TIMES = 1000;

void FaceAuthManagerTest::WriteFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_)
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
void FaceAuthManagerTest::WriteInitFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_)
{
    std::ofstream mystream("/data/init.dat", ios::trunc);
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

void FaceAuthManagerTest::WriteReleaseFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_)
{
    std::ofstream mystream("/data/release.dat", ios::trunc);
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
 * @tc.number: CPPAPI_Performance_Init_0100
 * @tc.name:: Performance of Init interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Performance_Init_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_Init_0100 start";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->Init();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_Init_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_Init_0100 end";
}
/**
 * @tc.number: CPPAPI_Performance_Release_0100
 * @tc.name:: Performance of Release interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Performance_Release_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_Release_0100 start";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        service->Init();
        auto startTime = std::chrono::high_resolution_clock::now();
        service->Release();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_Release_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_Release_0100 end";
}
/**
 * @tc.number: CPPAPI_Performance_Authenticate_0100
 * @tc.name:: Performance of Authenticate interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Performance_Authenticate_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_Authenticate_0100 start";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    param.reqId = 0;
    param.flags = -1;
    param.challenge = -1;
    param.faceId = 0;
    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        param.reqId++;
        callback->isCallback_ = false;
        auto startTime = std::chrono::high_resolution_clock::now();
        service->Authenticate(param, callback);
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
    }
    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_Authenticate_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_Authenticate_0100 end";
}

/**
 * @tc.number: CPPAPI_Performance_CancelAuth_0100
 * @tc.name:: Performance of CancelAuth interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Performance_CancelAuth_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_CancelAuth_0100 start";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    OHOS::sptr<TestCallback> callback(new TestCallback());
    uint64_t reqId = 0;
    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        reqId++;
        callback->isCallback_ = false;
        auto startTime = std::chrono::high_resolution_clock::now();
        service->CancelAuth(reqId, callback);
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }
    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_CancelAuth_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_CancelAuth_0100 end";
}

/**
 * @tc.number: CPPAPI_Performance_GetRemainingTime_0100
 * @tc.name:: Performance of GetRemainingTime interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Performance_GetRemainingTime_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_GetRemainingTime_0100 start";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->GetRemainingTime();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }
    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_GetRemainingTime_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_GetRemainingTime_0100 end";
}

/**
 * @tc.number: CPPAPI_Performance_GetRemainingNum_0100
 * @tc.name:: Performance of GetRemainingNum interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Performance_GetRemainingNum_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_GetRemainingNum_0100 start";
    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->GetRemainingNum();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_GetRemainingNum_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_GetRemainingNum_0100 end";
}

/**
 * @tc.number: CPPAPI_Performance_ResetTimeout_0100
 * @tc.name:: Performance of ResetTimeout interface
 * @tc.type: PERF
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Performance_ResetTimeout_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_ResetTimeout_0100 start";

    std::chrono::duration<double, std::milli> totalTime;
    std::chrono::duration<double, std::milli> averageTime;

    for (int32_t run_times = 0; run_times < PERFORMANCE_TEST_TIMES; run_times++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        service->ResetTimeout();
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
    }

    averageTime = totalTime / PERFORMANCE_TEST_TIMES;
    GTEST_LOG_(INFO) << "CPPAPI_Performance_ResetTimeout_0100  avg =";
    GTEST_LOG_(INFO) << averageTime.count();
    int32_t result = -1;
    if (averageTime.count() <= MAX_ELAPSED) {
        result = 0;
    }
    EXPECT_EQ(result, 0);
    GTEST_LOG_(INFO) << "FaceAuthManagerTest::CPPAPI_Performance_ResetTimeout_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_Init_0100
 * @tc.name:: Init Fail
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Init_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Init_0100 start";
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteInitFile(FACE_AUTH_FAIL, 0, resultInfos_);
    int32_t result = service->Init();
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Init_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_Init_0200
 * @tc.name:: Init Fail
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Init_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Init_0200 start";
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteInitFile(FACE_AUTH_INIT_TIMEOUT, 0, resultInfos_);
    int32_t result = service->Init();
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Init_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelAuth_0100
 * @tc.name:: Cancelauth is not called for authentication
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_CancelAuth_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0100 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 900;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t result = service->CancelAuth(param.reqId, callback);
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelAuth_0200
 * @tc.name:: Call cancelauth repeatedly
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_CancelAuth_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0200 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 1000;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    service->Authenticate(param, callback);
    std::this_thread::sleep_for(timebusy);
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    service->CancelAuth(param.reqId, callbackCancel);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_CANCEL);
    OHOS::sptr<TestCallback> callbackAuth(new TestCallback());
    int32_t result = service->CancelAuth(param.reqId, callbackAuth);
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    EXPECT_EQ(result, -1);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelAuth_0300
 * @tc.name:: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_CancelAuth_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0300 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 2100;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    result = service->CancelAuth(param.reqId, callbackCancel);
    if (result == 0) {
        while (!callbackCancel->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
    }
    EXPECT_EQ(callbackCancel->errorcode_, ERRCODE_SUCCESS);
    FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0300 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelAuth_0400
 * @tc.name:: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_CancelAuth_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0400 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 2200;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    service->Authenticate(param, callback);
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    int num = 10;
    int32_t result = service->CancelAuth(num, callbackCancel);
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    EXPECT_EQ(result, -1);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0400 end";
}

/**
 * @tc.number: CPPAPI_Function_CancelAuth_0500
 * @tc.name:: Cancel callback is nullptr
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_CancelAuth_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0500 start";
    OHOS::sptr<TestCallback> callbackCancel(new TestCallback());
    int num = 10;
    int32_t result = service->CancelAuth(num, nullptr);
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    EXPECT_EQ(result, -1);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_CancelAuth_0500 end";
}

/**
 * @tc.number: CPPAPI_Function_ResetTimeout_0100
 * @tc.name:: Unfreeze face authentication after freezing
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_ResetTimeout_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_ResetTimeout_0100 start";
    service->ResetTimeout();
    std::map<int32_t, ResultInfo> resultInfos_;
    AuthParam param;
    int32_t num = 5;
    uint64_t reqIdNum = 1200;
    for (int32_t i = 0; i < num; i++) {
        OHOS::sptr<TestCallback> callback(new TestCallback());
        ResultInfo info;
        info.resultCode = 18;
        int32_t range = 10;
        for (int32_t j = 0; j < range; j++) {
            info.param[j] = 0;
        }
        resultInfos_[0] = info;
        WriteFile(1, 1, resultInfos_);
        param.reqId = reqIdNum;
        param.flags = 0;
        param.challenge = 1;
        param.faceId = 0;
        int32_t result = service->Authenticate(param, callback);
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
        FACEAUTH_LABEL_LOGI("callback->code_ is %{public}d", callback->code_);
        FACEAUTH_LABEL_LOGI("callback->errorcode_ is %{public}d", callback->errorcode_);
        FACEAUTH_LABEL_LOGI("callback->reqid_ is %{public}d", callback->reqid_);
        FACEAUTH_LABEL_LOGI("callback->type_ is %{public}d", callback->type_);
        std::this_thread::sleep_for(waitcamera);
        result = service->GetRemainingNum();
        FACEAUTH_LABEL_LOGI("GetRemainingNum is %{public}d", result);
    }
    service->ResetTimeout();
    OHOS::sptr<TestCallback> callbackAuth(new TestCallback());
    int32_t result = service->Authenticate(param, callbackAuth);
    while (!callbackAuth->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callbackAuth->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_ResetTimeout_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_ResetTimeout_0200
 * @tc.name:: It is not frozen or unfrozen, and the number of attempts to obtain is still 5
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_ResetTimeout_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_ResetTimeout_0200 start";
    service->ResetTimeout();
    int32_t result = service->GetRemainingNum();
    EXPECT_EQ(result, AUTH_FAIL_MAX_TIMES);
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_ResetTimeout_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_GetRemainingNum_0100
 * @tc.name:: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_GetRemainingNum_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_GetRemainingNum_0100 start";
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
    int32_t numTwo = 2;
    int32_t numOne = 1;
    int32_t numZero = 0;
    CPPAPI_Function_GetRemainingNum_0100_Auth(numFour, resultInfos_);
    CPPAPI_Function_GetRemainingNum_0100_Auth(numThree, resultInfos_);
    CPPAPI_Function_GetRemainingNum_0100_Auth(numTwo, resultInfos_);
    CPPAPI_Function_GetRemainingNum_0100_Auth(numOne, resultInfos_);
    CPPAPI_Function_GetRemainingNum_0100_Auth(numZero, resultInfos_);
    service->ResetTimeout();
    std::this_thread::sleep_for(timebusy);
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_GetRemainingNum_0100 end";
}

void FaceAuthManagerTest::CPPAPI_Function_GetRemainingNum_0100_Auth(int32_t num,
    std::map<int32_t, ResultInfo> resultInfos_)
{
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 1600;
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
 * @tc.number: CPPAPI_Function_GetRemainingTime_0100
 * @tc.name:: Obtain the remaining unlocking time until unlocking, the time range is 0-30s
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_GetRemainingTime_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_GetRemainingTime_0100 start";
    std::map<int32_t, ResultInfo> resultInfos_;
    int32_t num = 5;
    uint64_t reqIdNum = 1800;
    for (int32_t i = 0; i < num; i++) {
        OHOS::sptr<TestCallback> callback(new TestCallback());
        ResultInfo info;
        info.resultCode = 18;
        int32_t range = 10;
        for (int32_t j = 0; j < range; j++) {
            info.param[j] = 0;
        }
        resultInfos_[0] = info;
        WriteFile(1, 1, resultInfos_);
        AuthParam param;
        param.reqId = reqIdNum;
        param.flags = 0;
        param.challenge = 1;
        param.faceId = 0;
        service->Authenticate(param, callback);
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
        std::this_thread::sleep_for(waitcamera);
    }
    long resultTime = service->GetRemainingTime();
    FACEAUTH_LABEL_LOGI("result is %{public}ld", resultTime);
    bool resultTimebool = false;
    if (resultTime > 0 && resultTime <= AUTH_FAIL_WAIT_TIME) {
        resultTimebool = true;
    }
    EXPECT_EQ(resultTimebool, true);
    service->ResetTimeout();
    GTEST_LOG_(INFO) << resultTime;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_GetRemainingTime_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_GetRemainingTime_0200
 * @tc.name:: Call the remaining unlocking time when it is not frozen, and return 0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_GetRemainingTime_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_GetRemainingTime_0200 start";
    service->ResetTimeout();
    int32_t result = service->GetRemainingTime();
    EXPECT_EQ(result, 0);
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_GetRemainingTime_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0100
 * @tc.name:: Face authentication failed
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0100 start";
    service->ResetTimeout();
    OHOS::sptr<TestCallback> callback(new TestCallback());
    std::map<int32_t, ResultInfo> resultInfos_;
    ResultInfo info;
    info.resultCode = 18;
    int32_t range = 10;
    for (int32_t j = 0; j < range; j++) {
        info.param[j] = 0;
    }
    resultInfos_[0] = info;
    WriteFile(1, 1, resultInfos_);
    AuthParam param;
    uint64_t reqIdNum = 2300;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_COMPARE_FAIL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0200
 * @tc.name:: Face authentication timeout
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0200 start";
    service->ResetTimeout();
    OHOS::sptr<TestCallback> callback(new TestCallback());
    std::map<int32_t, ResultInfo> resultInfos_;
    ResultInfo info;
    info.resultCode = 18;
    info.param[0] = 6;
    int32_t range = 10;
    for (int32_t j = 1; j < range; j++) {
        info.param[j] = 0;
    }
    resultInfos_[0] = info;
    WriteFile(1, 1, resultInfos_);
    AuthParam param;
    uint64_t reqIdNum = 2400;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_TIMEOUT);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0200 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0300
 * @tc.name:: Service busy during face authentication
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0300 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 2600;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    service->Authenticate(param, callback);
    OHOS::sptr<TestCallback> callback2(new TestCallback());
    int32_t result2 = service->Authenticate(param, callback2);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    while (!callback2->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback2->errorcode_, ERRCODE_BUSY);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result2;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0300 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0400
 * @tc.name:: Face authentication is locked because it has failed more than 5 times
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0400 start";
    service->ResetTimeout();

    std::map<int32_t, ResultInfo> resultInfos_;
    AuthParam param;
    int32_t num = 5;
    uint64_t reqIdNum = 2700;
    for (int32_t i = 0; i < num; i++) {
        OHOS::sptr<TestCallback> callback(new TestCallback());
        ResultInfo info;
        info.resultCode = 18;
        int32_t range = 10;
        for (int32_t j = 0; j < range; j++) {
            info.param[j] = 0;
        }
        resultInfos_[0] = info;
        WriteFile(1, 1, resultInfos_);
        param.reqId = reqIdNum;
        param.flags = 0;
        param.challenge = 1;
        param.faceId = 0;
        service->Authenticate(param, callback);
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
        std::this_thread::sleep_for(waitcamera);
        int32_t result = service->GetRemainingNum();
        FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    }
    OHOS::sptr<TestCallback> callbackAuth(new TestCallback());
    int32_t result = service->Authenticate(param, callbackAuth);
    while (!callbackAuth->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    FACEAUTH_LABEL_LOGI("result is %{public}d", result);
    EXPECT_EQ(callbackAuth->errorcode_, ERRCODE_IN_LOCKOUT_MODE);
    std::this_thread::sleep_for(waitcamera);
    service->ResetTimeout();
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0400 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0500
 * @tc.name:: Authentication can be performed normally after the lock is released
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0500 start";
    service->ResetTimeout();
    std::map<int32_t, ResultInfo> resultInfos_;
    AuthParam param;
    int32_t num = 5;
    uint64_t reqIdNum = 2900;
    for (int32_t i = 0; i < num; i++) {
        OHOS::sptr<TestCallback> callback(new TestCallback());
        ResultInfo info;
        info.resultCode = 18;
        int32_t range = 10;
        for (int32_t j = 0; j < range; j++) {
            info.param[j] = 0;
        }
        resultInfos_[0] = info;
        WriteFile(1, 1, resultInfos_);
        param.reqId = reqIdNum;
        param.flags = 0;
        param.challenge = 1;
        param.faceId = 0;
        service->Authenticate(param, callback);
        while (!callback->isCallback_) {
            std::this_thread::sleep_for(timeFast);
        }
        std::this_thread::sleep_for(waitcamera);
    }
    int32_t result = service->GetRemainingTime();
    bool resultTimebool = false;
    if (result > 0 && result <= AUTH_FAIL_WAIT_TIME) {
        resultTimebool = true;
    }
    EXPECT_EQ(resultTimebool, true);
    service->ResetTimeout();
    OHOS::sptr<TestCallback> callbackAuth(new TestCallback());
    result = service->Authenticate(param, callbackAuth);
    while (!callbackAuth->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callbackAuth->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0500 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0600
 * @tc.name:: Face authentication timeout is not included in the number of failures
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0600 start";
    service->ResetTimeout();
    OHOS::sptr<TestCallback> callback(new TestCallback());
    std::map<int32_t, ResultInfo> resultInfos_;
    ResultInfo info;
    info.resultCode = 18;
    info.param[0] = 6;
    int32_t range = 10;
    for (int32_t j = 1; j < range; j++) {
        info.param[j] = 0;
    }
    resultInfos_[0] = info;
    WriteFile(1, 1, resultInfos_);
    AuthParam param;
    uint64_t reqIdNum = 3000;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    std::this_thread::sleep_for(waitcamera);
    result = service->GetRemainingNum();
    EXPECT_EQ(result, 5);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0600 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0700
 * @tc.name:: Face authentication when challenge = - 1
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0700 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 3600;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = -1;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_INVALID_PARAMETERS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0700 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0800
 * @tc.name:: Face authentication when challenge = 0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0800 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 3700;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 0;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0800 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_0900
 * @tc.name:: Face authentication when challenge is greater than 0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0900 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 3800;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_0900 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_1000
 * @tc.name:: Face authentication when faceId = -1
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1000 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 3900;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = -1;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_INVALID_PARAMETERS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1000 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_1100
 * @tc.name:: Face authentication when faceId = 0
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_1100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1100 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 4000;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t result = service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    EXPECT_EQ(callback->errorcode_, ERRCODE_SUCCESS);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1100 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_1200
 * @tc.name:: Face authentication when callback = nullptr
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_1200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1200 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 4300;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = -1;
    int32_t result = service->Authenticate(param, nullptr);
    EXPECT_EQ(result, -1);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1200 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_1300
 * @tc.name:: Face authentication when GetResult File
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_1300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1300 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 1301;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(FACE_AUTH_GETRESULT_FAIL, 0, resultInfos_);
    service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    int32_t result = callback->errorcode_;
    EXPECT_EQ(result, ERRCODE_FAIL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1300 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_1400
 * @tc.name:: Face authentication when GetResult TimeOut
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_1400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1400 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 1401;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteFile(FACE_AUTH_GETRESULT_TIMEOUT, 0, resultInfos_);
    service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    int32_t result = callback->errorcode_;
    EXPECT_EQ(result, ERRCODE_TIMEOUT);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1400 end";
}

/**
 * @tc.number: CPPAPI_Function_Authenticate_1500
 * @tc.name:: Face authentication when carmera open fail
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Authenticate_1500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1500 start";
    OHOS::sptr<TestCallback> callback(new TestCallback());
    AuthParam param;
    uint64_t reqIdNum = 1501;
    param.reqId = reqIdNum;
    param.flags = 0;
    param.challenge = 1;
    param.faceId = 0;
    int32_t kill_times = 5;
    const auto temp_time = std::chrono::milliseconds(1000);
    while (kill_times) {
        FILE *ptr;
        ptr = popen("kill -9 $(pidof camera_service)", "r");
        pclose(ptr);
        kill_times--;
        std::this_thread::sleep_for(temp_time);
    }
    service->Authenticate(param, callback);
    while (!callback->isCallback_) {
        std::this_thread::sleep_for(timeFast);
    }
    int32_t result = callback->errorcode_;
    EXPECT_EQ(result, ERRCODE_CAMERA_FAIL);
    std::this_thread::sleep_for(waitcamera);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Authenticate_1500 end";
}

/**
 * @tc.number: CPPAPI_Function_Release_0100
 * @tc.name:: Release double times
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Release_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Release_0100 start";
    int32_t result = service->Release();
    result = service->Release();
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Release_0100 end";
}

/**
 * @tc.number: CPPAPI_Function_Release_0200
 * @tc.name:: Release Fail
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(FaceAuthManagerTest, CPPAPI_Function_Release_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Release_0200 start";
    std::map<int32_t, ResultInfo> resultInfos_;
    WriteReleaseFile(FACE_AUTH_FAIL, 0, resultInfos_);
    int32_t result = service->Release();
    EXPECT_EQ(result, -1);
    GTEST_LOG_(INFO) << result;
    GTEST_LOG_(INFO) << "FaceAuthManagerTest CPPAPI_Function_Release_0200 end";
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
