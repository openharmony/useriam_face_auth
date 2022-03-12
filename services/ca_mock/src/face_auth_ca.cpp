/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "face_auth_ca.h"
#include <cstdio>
#include <fstream>
#include <cstring>
#include "securec.h"
#include "defines.h"
#include "face_auth_func.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
static const int32_t CA_RESULT_SUCCESS = 0;
static const int32_t CA_RESULT_FAILED = 1;
static const int32_t CA_RESULT_CANCELED = 3;
static const int32_t CA_RESULT_LOCKED = 9;
static const int32_t BUFF_MAX_LEN = 128;
static const int32_t CASE_NUM = 100;
static const int32_t CODE_NUM = 5;
static const int32_t RESULT_CODE = 6;
static const int32_t IFACE_OVER_MAX_FACES = 901;
static const int32_t IFACE_ENROLL_HAS_REGISTERED = 902;
static const int32_t IFACE_CAMERA_FAIL = 903;
static const int32_t IFACE_TIME_OUT = 904;
static const std::string TEST_ANGLE = "999";
static const std::string TEST_ENROLL_SUCCESS = "998";
static const std::string TEST_OVER_MAX_FACES = "901";
static const std::string TEST_ENROLL_HAS_REGISTERED = "902";
static const std::string TEST_CAMERA_FAIL = "903";
static const std::string TEST_ANGLE_TIMEOUT = "905";
static const int32_t FACE_AUTH_GETRESULT_FAIL = 1001;
static const int32_t FACE_AUTH_GETRESULT_TIMEOUT = 1002;
static const std::string FACE_AUTH_GETRESULT_FAIL_STRING = "1001";
static const std::string FACE_AUTH_GETRESULT_TIMEOUT_STRING = "1002";
static const std::string FACE_AUTH_FAIL_STRING = "1003";
static const std::string FACE_AUTH_INIT_TIMEOUT_STRING = "1004";
static const char *INIT_FILENAME("/data/useriam/init.dat");
static const char *RELEASE_FILENAME("/data/useriam/release.dat");
static const char *CONFIG_FILENAME("/data/useriam/config.dat");
static const char *FACEID_FILENAME("/data/useriam/faceId.dat");
static const char *AUTH_RESULT_FILENAME("/data/useriam/auth_result.dat");
static const int32_t SLEEP_LONG_NUM = 2000;
static const int32_t PARAM_RANGE = 10;
static const int32_t TEST_ANGLT_START_NUM = 1000;
static const int32_t TEST_ANGLE_MAX_NUM = 1013;
static const int32_t TEST_ANGLE_ADD_NUM = 4;
static const int32_t DEFAULT_REMAIN_TIMES = 5;
static const int32_t SLEEP_TIME = 5000;
static int32_t faceId_ = 1;
static bool isAuthingFlag = false;
std::shared_ptr<FaceAuthCA> FaceAuthCA::faceAuthCA_ = nullptr;
std::mutex FaceAuthCA::mutex_;
FaceAuthCA::FaceAuthCA()
    : resultNum_(0), cancelReqId_(0), isReturnFaceId_(false), isInitFail_(false),
    isCancel_(false), type_(HW_EXEC_TYPE_LEARN)
{
    InitErrorCode();
}

std::shared_ptr<FaceAuthCA> FaceAuthCA::GetInstance()
{
    if (faceAuthCA_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (faceAuthCA_ == nullptr) {
            faceAuthCA_ = std::make_shared<FaceAuthCA>();
        }
    }
    return faceAuthCA_;
}

int32_t FaceAuthCA::Init()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    GenerateKeyPair();
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::Close()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::LoadAlgorithm()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    if (DynamicInit() == FI_RC_OK) {
        return CA_RESULT_SUCCESS;
    } else {
        return CA_RESULT_FAILED;
    }
}

int32_t FaceAuthCA::ReleaseAlgorithm()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    if (DynamicRelease() == FI_RC_OK) {
        return CA_RESULT_SUCCESS;
    } else {
        return CA_RESULT_FAILED;
    }
}

int32_t FaceAuthCA::StartAlgorithmOperation(AlgorithmOperation algorithmOperation, AlgorithmParam param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    isAuthingFlag = true;
    SetAlgorithmParam(param);
    algorithmOperation_ = algorithmOperation;
    if (algorithmOperation == Enroll) {
        Prepare(HW_EXEC_TYPE_ENROOL);
        return CA_RESULT_SUCCESS;
    } else if (algorithmOperation == Auth) {
        Prepare(HW_EXEC_TYPE_UNLOCK);
        return CA_RESULT_SUCCESS;
    } else {
        return CA_RESULT_FAILED;
    }
}

int32_t FaceAuthCA::TransferImageToAlgorithm(CameraImage images)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "TransferImageToAlgorithm");
    (void)(images);
    return CA_RESULT_SUCCESS;
}

void FaceAuthCA::GetAlgorithmState(int32_t &retCode, std::vector<uint8_t> &retCoauthMsg)
{
    retCoauthMsg.clear();
    int32_t algorithmState = -1;
    FICode code = CODE_CALLBACK_START;
    GetAuthState(algorithmState, code, param_.scheduleId);
    FACEAUTH_HILOGI(MODULE_SERVICE, "algorithmState = %{public}d.", algorithmState);
    retCoauthMsg.resize(sizeof(int32_t));
    if (memcpy_s(&retCoauthMsg[0], retCoauthMsg.size(), &algorithmState, sizeof(int32_t)) != EOK) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "memcpy_s fail");
        retCode = 1;
        return;
    }
    retCode = 1;
}

int32_t FaceAuthCA::GetExecutorInfo(std::vector<uint8_t> &pubKey, uint32_t &esl, uint64_t &authAbility)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    if (DoGetExecutorInfo(pubKey, esl, authAbility) != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "DoGetExecutorInfo failed.");
        return CA_RESULT_FAILED;
    }
    return CA_RESULT_SUCCESS;
}

int FaceAuthCA::getAlgorithmResult()
{
    if (isCancel_ == true) {
        return CA_RESULT_CANCELED;
    } else {
        if (algorithmOperation_ == Auth) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "face auth, remain times is %{public}d.",
                remainTimesMap_[param_.templateId]);
            if (remainTimesMap_[param_.templateId] == 0) {
                return CA_RESULT_LOCKED;
            }
        }
    }
    int authResult = 0;
    GetAuthResult(authResult);
    FACEAUTH_HILOGI(MODULE_SERVICE, "get authResult %{public}d.", authResult);
    return authResult;
}

int32_t FaceAuthCA::FinishAlgorithmOperation(AlgorithmResult &retResult)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s wait for image receive", __PRETTY_FUNCTION__);
    sleep(1);
    isAuthingFlag = false;
    FACEAUTH_HILOGI(MODULE_SERVICE, "isAuthingFlag = %{public}d.", isAuthingFlag);
    int32_t authResult = getAlgorithmResult();
    FACEAUTH_HILOGI(MODULE_SERVICE, "get auth result = %{public}d.", authResult);
    if (algorithmOperation_ == Enroll) {
        if (authResult == 0) {
            templateIdList_.push_back(param_.templateId);
            remainTimesMap_[param_.templateId] = DEFAULT_REMAIN_TIMES;
        }
    } else {
        if (authResult == 0) {
            remainTimesMap_[param_.templateId] = DEFAULT_REMAIN_TIMES;
        }
        if ((authResult == CA_RESULT_FAILED) && (remainTimesMap_[param_.templateId] > 0)) {
            remainTimesMap_[param_.templateId]--;
        }
    }
    isCancel_ = false;

    retResult.result = authResult;
    retResult.templateId = param_.templateId;
    retResult.remainTimes = remainTimesMap_[param_.templateId];

    Buffer *retTlv = CreateBuffer(RESULT_TLV_LEN);
    if (retTlv == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "CreateBuffer failed.");
        return CA_RESULT_FAILED;
    }
    ResultCode result = GenerateRetTlv(authResult, param_.scheduleId, FACE_2D, param_.templateId, retTlv);
    if (result != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GenerateRetTlv failed.");
        DestoryBuffer(retTlv);
        return CA_RESULT_FAILED;
    }
    result = SetResultTlv(retTlv, retResult.coauthMsg);
    if (result != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "SetResultTlv failed.");
        DestoryBuffer(retTlv);
        return CA_RESULT_FAILED;
    }
    DestoryBuffer(retTlv);
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::DeleteTemplate(uint64_t templateId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    std::vector<uint64_t>::const_iterator iter;
    for (iter = templateIdList_.cbegin(); iter != templateIdList_.cend(); ++iter) {
        if (*iter == templateId) {
            break;
        }
    }
    if (iter == templateIdList_.cend()) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "%{public}s template to delete not found.", __PRETTY_FUNCTION__);
        return CA_RESULT_FAILED;
    }
    templateIdList_.erase(iter);
    remainTimesMap_.erase(templateId);
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::VerifyTemplateData(std::vector<uint64_t> templateIdList)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    templateIdList_.assign(templateIdList.begin(), templateIdList.end());
    remainTimesMap_.clear();
    for (uint32_t index = 0; index < templateIdList_.size(); index++) {
        remainTimesMap_[templateIdList_.at(index)] = DEFAULT_REMAIN_TIMES;
    }
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::GetFaceInfo(uint64_t templateId, FaceCredentialInfo &faceCredentialInfo)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    faceCredentialInfo.subType = FACE_2D;
    faceCredentialInfo.freezingTime = 0;
    faceCredentialInfo.remainTimes = remainTimesMap_[templateId];
    return 0;
}

int32_t FaceAuthCA::GetRemainTimes(uint64_t templateId, int32_t &remainingTimes)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    remainingTimes = remainTimesMap_[templateId];
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::ResetRemainTimes(uint64_t templateId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s reset times templateId %{public}s.", __PRETTY_FUNCTION__,
        getMaskedString(templateId).c_str());
    remainTimesMap_[templateId] = DEFAULT_REMAIN_TIMES;
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::FreezeTemplate(uint64_t templateId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s freeze template %{public}s.", __PRETTY_FUNCTION__,
        getMaskedString(templateId).c_str());
    remainTimesMap_[templateId] = 0;
    return CA_RESULT_SUCCESS;
}

int32_t FaceAuthCA::CancelAlgorithmOperation()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    if (Cancel(param_.scheduleId) != FI_RC_OK) {
        return CA_RESULT_FAILED;
    }

    return CA_RESULT_SUCCESS;
}

void FaceAuthCA::SetAlgorithmParam(const AlgorithmParam &param)
{
    param_ = param;
}

void FaceAuthCA::GetAuthResult(int32_t &result)
{
    FILE *file = nullptr;
    file = fopen(AUTH_RESULT_FILENAME, "r");
    if (file == nullptr) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "open file failed.");
        return;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "fseek failed.");
        fclose(file);
        return;
    }
    if (ftell(file) > 0) {
        fseek(file, 0, SEEK_SET);
        char str[BUFF_MAX_LEN] = {0};
        fread((void *)str, sizeof(char), BUFF_MAX_LEN - 1, file);
        result = atoi(str);
    }
    if (fclose(file) != 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "fclose failed.");
    }
}
FIRetCode FaceAuthCA::GetAuthState(int32_t &authErrorCode, FICode &code, uint64_t reqId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "GetAuthState start");
    int32_t resultCode = 0;
    int32_t params[ALO_GETRESULT_PARAM_LEN] = {0};
    if (!CheckIsCancel(authErrorCode, code, reqId)) {
        return FI_RC_OK;
    }
    FIRetCode result = GetState(resultCode, params);
    if (result != FI_RC_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetState failed");
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "params[0] is %{public}d", params[0]);
    if (resultCode == FI_COMPARE_FAIL) {
        authErrorCode = SwitchAuthErrorCode(params[0]);
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (errorCode_.find(resultCode) != errorCode_.end()) {
        authErrorCode = errorCode_[resultCode];
    } else {
        authErrorCode = resultCode;
    }
    if (authErrorCode == FACE_AUTH_GETRESULT_FAIL) {
        authErrorCode = ERRCODE_FAIL;
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (authErrorCode == FACE_AUTH_GETRESULT_TIMEOUT) {
        authErrorCode = ERRCODE_TIMEOUT;
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (authErrorCode == IFACE_TIME_OUT) {
        authErrorCode = ERRCODE_TIMEOUT;
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (authErrorCode == IFACE_CAMERA_FAIL) {
        authErrorCode = ERRCODE_CAMERA_FAIL;
    }
    if (authErrorCode == ERRCODE_SUCCESS || authErrorCode == ERRCODE_CAMERA_FAIL) {
        code = CODE_CALLBACK_RESULT;
    } else {
        code = CODE_CALLBACK_ACQUIRE;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "reqid_ is %{public}s, code_ is %{public}d, errorcode_ is %{public}d",
        getMaskedString(reqId).c_str(), code, authErrorCode);
    FACEAUTH_HILOGI(MODULE_SERVICE, "GetAuthState end");
    return result;
}

FIRetCode FaceAuthCA::GetState(int32_t &resultCode, int32_t (&param)[RESULT_MAX_SIZE])
{
    ReadFile();
    FACEAUTH_HILOGI(MODULE_SERVICE, "resultNum_ is %{public}d", resultNum_);
    FACEAUTH_HILOGI(MODULE_SERVICE, "resultInfos_.size() is %{public}d", resultInfos_.size());
    if (resultNum_ < (int)resultInfos_.size()) {
        resultCode = resultInfos_[resultNum_].resultCode;
        FACEAUTH_HILOGI(MODULE_SERVICE, "memcpy length is %{public}d", sizeof(int32_t) * RESULT_MAX_SIZE);
        if (memcpy_s(param, sizeof(param), resultInfos_[resultNum_].param,
            sizeof(resultInfos_[resultNum_].param)) != EOK) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "memcpy_s fail");
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
        FACEAUTH_HILOGI(MODULE_SERVICE, "authErrorCode is %{public}d", resultCode);
        if (param[0] != 0) {
            for (int32_t i = 0; i < RESULT_MAX_SIZE; i++) {
                FACEAUTH_HILOGI(MODULE_SERVICE, "param[%{public}d] is %{public}d", i, param[i]);
            }
        }
    }
    return FIRetCode::FI_RC_OK;
}
void FaceAuthCA::ReadFile()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "ReadFile start");
    resultInfos_.clear();
    std::ifstream mystream(CONFIG_FILENAME);

    if (!mystream.is_open()) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Prepare type is %{public}d", type_);
        if (type_ == HW_EXEC_TYPE_ENROOL) {
            resultInfos_[0].resultCode = FI_ENROLL_SUCCESS;
            resultInfos_[1].resultCode = FI_FACE_SCALE_TOO_SMALL;
        } else {
            resultInfos_[0].resultCode = FI_COMPARE_SUCCESS;
            resultInfos_[1].resultCode = FI_FACE_OFFSET_RIGHT;
        }
        FACEAUTH_HILOGI(MODULE_SERVICE, "ReadFile open fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_HILOGI(MODULE_SERVICE, "casenum is %{public}s", s.c_str());
    CheckFile(s);
    char buffer[CODE_NUM];
    mystream.getline(buffer, CODE_NUM);
    char str[RESULT_CODE];
    int32_t num = atoi(buffer);
    for (int32_t i = 0; i < num; i++) {
        mystream >> str;
        resultInfos_[i].resultCode = atoi(str);
        FACEAUTH_HILOGI(MODULE_SERVICE, "authErrorCode is %{public}d", resultInfos_[i].resultCode);
        for (int32_t j = 0; j < PARAM_NUM; j++) {
            mystream >> str;
            resultInfos_[i].param[j] = atoi(str);
        }
    }
    mystream.close();
}
int32_t FaceAuthCA::SwitchAuthErrorCode(int32_t param)
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
int32_t FaceAuthCA::CheckIsCancel(int32_t &authErrorCode, FICode &code, uint64_t reqId)
{
    if (isCancel_) {
        if (cancelReqId_ != reqId) {
            isCancel_ = false;
            FACEAUTH_HILOGW(MODULE_SERVICE,
                "cancelReqId_ and reqId are different. cancelReqId_ is %{public}s, reqId is %{public}s",
                getMaskedString(cancelReqId_).c_str(), getMaskedString(reqId).c_str());
        }
        authErrorCode = ERRCODE_CANCEL;
        code = CODE_CALLBACK_RESULT;
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        return FI_RC_OK;
    }
    return FI_RC_ERROR;
}
FIRetCode FaceAuthCA::Cancel(uint64_t reqId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "isAuthingFlag is %{public}d", isAuthingFlag);
    if (isAuthingFlag) {
        isCancel_ = true;
        cancelReqId_ = reqId;
        remove(CONFIG_FILENAME);
        return FI_RC_OK;
    }
    return FI_RC_ERROR;
}
void FaceAuthCA::InitErrorCode()
{
    errorCode_[FI_COMPARE_SUCCESS] = ERRCODE_SUCCESS;
    errorCode_[FI_FACE_NOT_FOUND] = FACE_NOT_FOUND;
    errorCode_[FI_FACE_SCALE_TOO_SMALL] = FACE_SCALE_TOO_SMALL;
    errorCode_[FI_FACE_SCALE_TOO_LARGE] = FACE_SCALE_TOO_LARGE;
    errorCode_[FI_FACE_OFFSET_LEFT] = FACE_OFFSET_LEFT;
    errorCode_[FI_FACE_OFFSET_TOP] = FACE_OFFSET_TOP;
    errorCode_[FI_FACE_OFFSET_RIGHT] = FACE_OFFSET_RIGHT;
    errorCode_[FI_FACE_OFFSET_BOTTOM] = FACE_OFFSET_BOTTOM;
    errorCode_[MG_UNLOCK_ATTR_EYE_OCCLUSION] = FACE_EYE_OCCLUSION;
    errorCode_[MG_UNLOCK_ATTR_EYE_CLOSE] = FACE_EYE_CLOSE;
    errorCode_[MG_UNLOCK_ATTR_MOUTH_OCCLUSION] = FACE_EYE_MOUTH_OCCLUSION;
    errorCode_[MG_UNLOCK_FACE_NOT_COMPLETE] = FACE_NOT_COMPLETE;
    errorCode_[FI_FACE_TOO_DARK] = FACE_UNLOCK_FACE_DARKLIGHT;
    errorCode_[MG_UNLOCK_HIGHLIGHT] = FACE_UNLOCK_FACE_HIGHTLIGHT;
    errorCode_[MG_UNLOCK_HALF_SHADOW] = FACE_UNLOCK_FACE_HALF_SHADOW;
    errorCode_[FI_NOT_GAZE] = FACE_NOT_GAZE;
    errorCode_[FI_FACE_ROTATE_TOP_RIGHT] = FACE_ROTATE_TOP_RIGHT;
    errorCode_[FI_FACE_ROTATE_TOP_LEFT] = FACE_ROTATE_TOP_LEFT;
    errorCode_[FI_FACE_ROTATE_BOTTOM_RIGHT] = FACE_ROTATE_BOTTOM_RIGHT;
    errorCode_[FI_FACE_ROTATE_BOTTOM_LEFT] = FACE_ROTATE_BOTTOM_LEFT;
    errorCode_[FI_FACE_FACE_SHADE] = FACE_WITHOUT_MASK;
    errorCode_[FI_FACE_INFO] = FACE_COVERED_WITH_MASK;
    errorCode_[MG_UNLOCK_FACE_BAD_QUALITY] = FACE_BAD_QUALITY;
    errorCode_[FI_COMPARE_FAIL] = FACE_LIVENESS_FAILURE;
    errorCode_[FI_FACE_ROTATE_LEFT] = FACE_ROTATED_LEFT;
    errorCode_[MG_UNLOCK_FACE_RISE] = FACE_RISE;
    errorCode_[MG_UNLOCK_FACE_ROTATED_RIGHT] = FACE_ROTATED_RIGHT;
    errorCode_[MG_UNLOCK_FACE_DOWN] = FACE_DOWN;
    errorCode_[MG_UNLOCK_FACE_MULTI] = FACE_MULTI;
    errorCode_[MG_UNLOCK_FACE_BLUR] = FACE_BLUR;
    errorCode_[FI_ENROLL_FACE_ANGLE_OK] = FACE_ANGLE_BASE;
    errorCode_[FI_FACE_INFO] = FACE_ENROLL_INFO_BEGIN;
    errorCode_[FI_ENROLL_SUCCESS] = ERRCODE_SUCCESS;
    errorCode_[FI_FACE_DETECTED] = FACE_DETECTED;
    errorCode_[FI_FACE_OUT_OF_BOUNDS] = FACE_OUT_OF_BOUNDS;
    errorCode_[FI_FACE_DARKPIC] = FACE_DARKPIC;
    errorCode_[IFACE_ENROLL_HAS_REGISTERED] = ERRCODE_ENROLL_HAS_REGISTERED;
}
void FaceAuthCA::CheckFile(std::string s)
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
void FaceAuthCA::GetEnrollAngleResult()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "GetEnrollAngleResult start");
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
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetEnrollAngleResult open");
        mystream << (std::to_string(faceId_) + "\n");
        info.param[0] = faceId_;
        faceId_++;
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetEnrollAngleResult exist");
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
void FaceAuthCA::GetEnrollSuccessResult()
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
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetEnrollSuccessResult open");
        mystream << (std::to_string(faceId_) + "\n");
        info.param[0] = faceId_;
        faceId_++;
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetEnrollSuccessResult exist");
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
void FaceAuthCA::GetEnrollHasRegistered()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = IFACE_ENROLL_HAS_REGISTERED;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}

void FaceAuthCA::GetOverMaxFace()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = IFACE_OVER_MAX_FACES;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}

void FaceAuthCA::GetCaremaFail()
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
void FaceAuthCA::GetAngleTimeout()
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
void FaceAuthCA::GetResultFail()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = FACE_AUTH_GETRESULT_FAIL;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}

void FaceAuthCA::GetResultTimeOut()
{
    resultInfos_.clear();
    ResultInfo info;
    info.resultCode = FACE_AUTH_GETRESULT_TIMEOUT;
    resultInfos_.insert(std::pair<int32_t, ResultInfo>(0, info));
}
FIRetCode FaceAuthCA::DynamicInit()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "DynamicInit");
    ReadInitFile();
    FACEAUTH_HILOGI(MODULE_SERVICE, "DynamicInit isInitFail_ is %{public}d", isInitFail_);
    if (isInitFail_) {
        isInitFail_ = false;
        remove(INIT_FILENAME);
        return FIRetCode::FI_RC_ERROR;
    }
    return FIRetCode::FI_RC_OK;
}

FIRetCode FaceAuthCA::DynamicRelease()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "DynamicRelease");
    ReadReleaseFile();
    FACEAUTH_HILOGI(MODULE_SERVICE, "DynamicInit isInitFail_ is %{public}d", isInitFail_);
    if (isInitFail_) {
        isInitFail_ = false;
        remove(RELEASE_FILENAME);
        return FIRetCode::FI_RC_ERROR;
    }
    return FIRetCode::FI_RC_OK;
}
void FaceAuthCA::ReadInitFile()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "ReadInitFile start");
    std::ifstream mystream(INIT_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "ReadInitFile open fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_HILOGI(MODULE_SERVICE, "casenum is %{public}s", s.c_str());
    CheckInitFile(s);
    mystream.close();
    return;
}

void FaceAuthCA::ReadReleaseFile()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "ReadReleaseFile start");
    std::ifstream mystream(RELEASE_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "ReadReleaseFile open fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_HILOGI(MODULE_SERVICE, "casenum is %{public}s", s.c_str());
    CheckReleaseFile(s);
    mystream.close();
    return;
}
void FaceAuthCA::CheckInitFile(std::string s)
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

void FaceAuthCA::CheckReleaseFile(std::string s)
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
FIRetCode FaceAuthCA::Prepare(HWExeType type)
{
    std::mutex mt;
    std::lock_guard<std::mutex> lock_l(mt);
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthAlgoImpl::Prepare");
    type_ = type;
    resultNum_ = 0;
    resultInfos_.clear();
    FACEAUTH_HILOGI(MODULE_SERVICE, "resultInfos_.size is %{public}d", resultInfos_.size());
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthAlgoImpl::Prepare type is %{public}d", type_);
    return FIRetCode::FI_RC_OK;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
