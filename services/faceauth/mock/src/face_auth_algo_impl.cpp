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

#include "face_auth_algo_impl.h"
#include <securec.h>
#include <thread>
#include <fstream>
#include "nlohmann/json.hpp"
#include "faceauth_log_wrapper.h"
#include "face_auth_defines.h"
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthAlgoImpl::mutex_;
std::shared_ptr<FaceAuthAlgoImpl> FaceAuthAlgoImpl::instance_ = nullptr;
static int32_t faceId_ = 1;
static const int32_t SLEEP_NUM = 2500;
static const int32_t SLEEP_LONG_NUM = 2000;
static const int32_t PARAM_RANGE = 10;
static const int32_t TEST_ANGLT_START_NUM = 1000;
static const int32_t TEST_ANGLE_MAX_NUM = 1013;
static const int32_t TEST_ANGLE_ADD_NUM = 4;
std::shared_ptr<FaceAuthAlgoImpl> FaceAuthAlgoImpl::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthAlgoImpl>();
        }
    }
    return instance_;
}

FaceAuthAlgoImpl::FaceAuthAlgoImpl()
    : resultNum_(0), isReturnFaceId_(false), type_(HW_EXEC_TYPE_LEARN), isInitFail_(false)
{}

FaceAuthAlgoImpl::~FaceAuthAlgoImpl()
{}

FIRetCode FaceAuthAlgoImpl::Init(int32_t imageWidth, int32_t imageHeight, const std::function<void()> &callback)
{
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::DynamicInit()
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl DynamicInit");
    ReadInitFile();
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl DynamicInit isInitFail_ is %{public}d", isInitFail_);
    if (isInitFail_) {
        isInitFail_ = false;
        remove(INIT_FILENAME);
        return FIRetCode::FI_RC_ERROR;
    }
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::DynamicRelease()
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl DynamicRelease");
    ReadReleaseFile();
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl DynamicInit isInitFail_ is %{public}d", isInitFail_);
    if (isInitFail_) {
        isInitFail_ = false;
        remove(RELEASE_FILENAME);
        return FIRetCode::FI_RC_ERROR;
    }
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::GetEnrollPoseCount(int32_t &poseCount)
{
    poseCount = AHGLE_DIM;
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::Prepare(HWExeType type, std::unique_ptr<PrepareParam> param)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl::Prepare");
    type_ = type;
    resultNum_ = 0;
    resultInfos_.clear();
    FACEAUTH_LABEL_LOGI("resultInfos_.size is %{public}d", resultInfos_.size());
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl::Prepare type is %{public}d", type_);
    return FIRetCode::FI_RC_OK;
}
FIRetCode FaceAuthAlgoImpl::TranformImage(std::unique_ptr<CameraImageStream> img, int32_t count)
{
    FACEAUTH_LABEL_LOGI("img->size is %{public}d", img->size);
    return FIRetCode::FI_RC_OK;
}
FIRetCode FaceAuthAlgoImpl::GetResult(int32_t &resultCode, int32_t param[RESULT_MAX_SIZE])
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_NUM));
    ReadFile();
    FACEAUTH_LABEL_LOGI("resultNum_ is %{public}d", resultNum_);
    FACEAUTH_LABEL_LOGI("resultInfos_.size() is %{public}d", resultInfos_.size());
    if (resultNum_ < resultInfos_.size()) {
        resultCode = resultInfos_[resultNum_].resultCode;
        FACEAUTH_LABEL_LOGI("memcpy_s length is %{public}d", sizeof(int32_t) * RESULT_MAX_SIZE);
        if (memcpy_s(param, sizeof(int32_t) * RESULT_MAX_SIZE, resultInfos_[resultNum_].param,
                sizeof(int32_t) * RESULT_MAX_SIZE) != EOK) {
            return FIRetCode::FI_RC_ERROR;
        }
        if (resultNum_ >= 1 && resultInfos_[resultNum_ - 1].resultCode == FACE_DETECTED) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_LONG_NUM));
        }
        resultNum_++;
        int32_t result = 0;

        if (resultCode == FI_COMPARE_FAIL || param[0] != 0) {
            result = SwitchAuthErrorCode(param[0]);
        }
        int32_t num = 6;
        if (resultCode == FI_COMPARE_FAIL && param[0] == num) {
            resultCode = IFACE_TIME_OUT;
            resultInfos_.clear();
            resultNum_ = 0;
            remove(CONFIG_FILENAME);
            return FIRetCode::FI_RC_OK;
        }
        if (resultCode == FI_ENROLL_SUCCESS && param[0] != 0 && isReturnFaceId_ == false) {
            isReturnFaceId_ = true;
            resultNum_--;
            return FIRetCode::FI_RC_OK;
        }

        if ((resultCode >= FI_ENROLL_SUCCESS && resultCode <= FI_COMPARE_FAIL) || resultCode == IFACE_OVER_MAX_FACES ||
            resultCode == IFACE_ENROLL_HAS_REGISTERED || resultCode == FACE_AUTH_GETRESULT_FAIL ||
            resultCode == FACE_AUTH_GETRESULT_TIMEOUT) {
            if (result != FACE_MOVED) {
                resultInfos_.clear();
                resultNum_ = 0;
                remove(CONFIG_FILENAME);
            }
        }
        FACEAUTH_LABEL_LOGI("authErrorCode is %{public}d", resultCode);
        if (param[0] != 0) {
            for (int32_t i = 0; i < RESULT_MAX_SIZE; i++) {
                FACEAUTH_LABEL_LOGI("param[%{public}d] is %{public}d", i, param[i]);
            }
        }
    }
    return FIRetCode::FI_RC_OK;
}

int32_t FaceAuthAlgoImpl::SwitchAuthErrorCode(int32_t param)
{
    int32_t authErrorCode = 0;
    switch (param) {
        case FI_FAIL_REASON_FACE_NOT_MATCH:
            authErrorCode = ERRCODE_COMPARE_FAIL;
            break;
        case FI_FAIL_REASON_FACE_MOVED:
            authErrorCode = FACE_MOVED;
            break;
        case FI_FAIL_REASON_NOT_GAZE:
            authErrorCode = ERRCODE_NOT_GAZE;
            break;
        case FI_FAIL_REASON_TIME_OUT:
            authErrorCode = ERRCODE_TIMEOUT;
            break;
        case FI_FAIL_REASON_FACE_OUT_OF_BOUNDS:
        case FI_FAIL_REASON_FACE_NOT_FOUND:
        case FI_FAIL_REASON_LIVENESS_FAILURE:
            authErrorCode = ERRCODE_FAIL;
            break;
        default:
            authErrorCode = ERRCODE_NO_FACE_DATA;
            break;
    }
    return authErrorCode;
}

FIRetCode FaceAuthAlgoImpl::Cancel()
{
    remove(CONFIG_FILENAME);
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::Reset(uint32_t errorCode)
{
    FACEAUTH_LABEL_LOGI("errorCode is %{public}d", errorCode);
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::ModelUpgrade()
{
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::DeleteFace(uint32_t faceId)
{
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::DeInit()
{
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::SendCommand(int32_t cmd, const uint8_t &params, uint32_t paramsSize)
{
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthAlgoImpl::CheckNeedUpgradeFeature(bool &needUpgrade)
{
    needUpgrade = true;
    return FIRetCode::FI_RC_OK;
}

void FaceAuthAlgoImpl::ReadFile()
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl ReadFile start");
    resultInfos_.clear();
    std::ifstream mystream(CONFIG_FILENAME);

    if (!mystream.is_open()) {
        FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl::Prepare type is %{public}d", type_);
        if (type_ == HW_EXEC_TYPE_ENROOL) {
            resultInfos_[0].resultCode = FI_FACE_SCALE_TOO_SMALL;
            resultInfos_[1].resultCode = FI_ENROLL_SUCCESS;
        } else {
            resultInfos_[0].resultCode = FI_FACE_OFFSET_RIGHT;
            resultInfos_[1].resultCode = FI_COMPARE_SUCCESS;
        }
        FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl ReadFile open fail");

        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_LABEL_LOGI("casenum is %{public}s", s.c_str());
    CheckFile(s);
    char buffer[CODE_NUM];
    mystream.getline(buffer, CODE_NUM);
    char str[RESULT_CODE];
    int32_t num = atoi(buffer);
    for (int32_t i = 0; i < num; i++) {
        mystream >> str;
        resultInfos_[i].resultCode = atoi(str);
        FACEAUTH_LABEL_LOGI("authErrorCode is %{public}d", resultInfos_[i].resultCode);
        for (int32_t j = 0; j < PARAM_NUM; j++) {
            mystream >> str;
            resultInfos_[i].param[j] = atoi(str);
        }
    }
    mystream.close();
}

void FaceAuthAlgoImpl::CheckFile(std::string s)
{
    if (s.compare(TEST_ANGLE) == 0) {
        GetEnrollAngleResult();
    }
    if (s.compare(TEST_ENROLL_SUCCESS) == 0) {
        GetEnrollSuccessResult();
    }
    if (s.compare(TEST_ENROLL_HAS_REGISTERED) == 0) {
        GetEnrollHasRegistered();
    }
    if (s.compare(TEST_OVER_MAX_FACES) == 0) {
        GetOverMaxFace();
    }
    if (s.compare(TEST_CAMERA_FAIL) == 0) {
        GetCaremaFail();
    }
    if (s.compare(TEST_ANGLE_TIMEOUT) == 0) {
        GetAngleTimeout();
    }
    if (s.compare(FACE_AUTH_GETRESULT_FAIL_STRING) == 0) {
        GetResultFail();
    }
    if (s.compare(FACE_AUTH_GETRESULT_TIMEOUT_STRING) == 0) {
        GetResultTimeOut();
    }
}

void FaceAuthAlgoImpl::GetResultFail()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = FACE_AUTH_GETRESULT_FAIL;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}

void FaceAuthAlgoImpl::GetResultTimeOut()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = FACE_AUTH_GETRESULT_TIMEOUT;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}

void FaceAuthAlgoImpl::GetAngleTimeout()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = FI_FACE_DETECTED;
    for (int32_t j = 0; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
    int32_t num = 1004;
    info.resultCode = num;
    for (int32_t j = 0; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(1, info));

    info.resultCode = FACE_HAS_REGISTERED;
    for (int32_t j = 0; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    int32_t numTwo = 2;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(numTwo, info));

    info.resultCode = FI_COMPARE_FAIL;
    int32_t infoNum = 6;
    info.param[0] = infoNum;
    for (int32_t j = 1; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    int32_t num1 = 3;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(num1, info));
}

void FaceAuthAlgoImpl::GetEnrollHasRegistered()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = IFACE_ENROLL_HAS_REGISTERED;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}

void FaceAuthAlgoImpl::GetOverMaxFace()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = IFACE_OVER_MAX_FACES;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}

void FaceAuthAlgoImpl::GetCaremaFail()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = IFACE_CAMERA_FAIL;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
    info.resultCode = FI_ENROLL_FAIL;
    for (int32_t j = 0; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(1, info));
}

void FaceAuthAlgoImpl::GetEnrollSuccessResult()
{
    resultInfos_.clear();
    ResultInfo info;
    int mapNum = 0;
    std::ofstream mystream(FACEID_FILENAME, std::ios::trunc);
    info.resultCode = FI_FACE_DETECTED;
    for (int32_t j = 0; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(mapNum, info));
    mapNum++;
    if (mystream.is_open()) {
        FACEAUTH_LABEL_LOGE("FaceAuthAlgoImpl GetEnrollSuccessResult open");
        mystream << (std::to_string(faceId_) + "\n");
        info.param[0] = faceId_;
        faceId_++;
    } else {
        FACEAUTH_LABEL_LOGE("FaceAuthAlgoImpl GetEnrollSuccessResult exist");
        mystream << (std::to_string(1) + "\n");
        info.param[0] = 1;
    }
    mystream.close();
    for (int32_t j = 1; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    info.resultCode = FI_ENROLL_SUCCESS;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(mapNum, info));
}

void FaceAuthAlgoImpl::GetEnrollAngleResult()
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl GetEnrollAngleResult start");
    resultInfos_.clear();
    int mapNum = 0;
    ResultInfo info;
    info.resultCode = FACE_DETECTED;
    for (int32_t j = 0; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(mapNum, info));
    for (int32_t i = TEST_ANGLT_START_NUM; i < TEST_ANGLE_MAX_NUM; i += TEST_ANGLE_ADD_NUM) {
        info.resultCode = i;
        for (int32_t j = 0; j < PARAM_RANGE; j++) {
            info.param[j] = 0;
        }
        resultInfos_.insert(std::pair<int32_t, ResultInfo>(mapNum, info));
        mapNum++;
    }
    std::ofstream mystream(FACEID_FILENAME, std::ios::trunc);
    if (mystream.is_open()) {
        FACEAUTH_LABEL_LOGE("FaceAuthAlgoImpl GetEnrollAngleResult open");
        mystream << (std::to_string(faceId_) + "\n");
        info.param[0] = faceId_;
        faceId_++;
    } else {
        FACEAUTH_LABEL_LOGE("FaceAuthAlgoImpl GetEnrollAngleResult exist");
        mystream << (std::to_string(1) + "\n");
        info.param[0] = 1;
    }
    mystream.close();
    for (int32_t j = 1; j < PARAM_RANGE; j++) {
        info.param[j] = 0;
    }
    info.resultCode = FI_ENROLL_SUCCESS;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(mapNum, info));
}

void FaceAuthAlgoImpl::ReadInitFile()
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl ReadInitFile start");
    std::ifstream mystream(INIT_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl ReadInitFile open fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_LABEL_LOGI("casenum is %{public}s", s.c_str());
    CheckInitFile(s);
    mystream.close();
    return;
}

void FaceAuthAlgoImpl::ReadReleaseFile()
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl ReadReleaseFile start");
    std::ifstream mystream(RELEASE_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_LABEL_LOGI("FaceAuthAlgoImpl ReadReleaseFile open fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_LABEL_LOGI("casenum is %{public}s", s.c_str());
    CheckReleaseFile(s);
    mystream.close();
    return;
}

void FaceAuthAlgoImpl::CheckInitFile(std::string s)
{
    if (s.compare(FACE_AUTH_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_INIT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_FAIL_STRING) == 0) {
        GetResultFail();
    }
    if (s.compare(FACE_AUTH_GETRESULT_TIMEOUT_STRING) == 0) {
        GetResultTimeOut();
    }
}

void FaceAuthAlgoImpl::CheckReleaseFile(std::string s)
{
    if (s.compare(FACE_AUTH_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_INIT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_FAIL_STRING) == 0) {
        GetResultFail();
    }
    if (s.compare(FACE_AUTH_GETRESULT_TIMEOUT_STRING) == 0) {
        GetResultTimeOut();
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS