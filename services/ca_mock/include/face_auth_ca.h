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
#ifndef FACE_AUTH_CA_H
#define FACE_AUTH_CA_H
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include "face_auth_defines.h"
#include "face_auth_log_wrapper.h"
#include "face_auth_defines.h"
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
typedef enum {
    Enroll,
    Auth,
} AlgorithmOperation;

typedef struct {
    uint64_t templateId = 0;
    uint64_t scheduleId = 0;
} AlgorithmParam;

typedef struct {
    uint8_t *image = nullptr;
    uint32_t imageSize = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t stride = 0;
    int64_t timestamp = 0;
} CameraImage;

typedef struct {
    int32_t result = 0;
    int32_t remainTimes = 0;
    uint64_t templateId = 0;
    std::vector<uint8_t> coauthMsg; // coauth signed msg
} AlgorithmResult;

typedef struct {
    int32_t resultCode;
    int32_t param[RESULT_MAX_SIZE];
} ResultInfo;

typedef struct FaceCredentialInfo {
    uint64_t subType;
    uint32_t remainTimes;
    uint32_t freezingTime;
} FaceCredentialInfo;

class FaceAuthCA {
public:
    static std::shared_ptr<FaceAuthCA> GetInstance();
    FaceAuthCA();
    ~FaceAuthCA()=default;
    int32_t Init();
    int32_t Close();
    int32_t LoadAlgorithm();
    int32_t ReleaseAlgorithm();
    int32_t StartAlgorithmOperation(AlgorithmOperation algorithmOperation, AlgorithmParam param);
    int32_t TransferImageToAlgorithm(CameraImage images);
    void GetAlgorithmState(int32_t &retCode, std::vector<uint8_t> &retCoauthMsg);
    int32_t GetExecutorInfo(std::vector<uint8_t> &pubKey, uint32_t &esl, uint64_t &authAbility);
    int32_t FinishAlgorithmOperation(AlgorithmResult &retResult);
    int32_t DeleteTemplate(uint64_t templateId);
    int32_t VerifyTemplateData(std::vector<uint64_t> templateIdList);
    int32_t GetRemainTimes(uint64_t templateId, int32_t &remainingTimes);
    int32_t GetFaceInfo(uint64_t templateId, FaceCredentialInfo &faceCredentialInfo);
    int32_t ResetRemainTimes(uint64_t templateId);
    int32_t FreezeTemplate(uint64_t templateId);
    int32_t CancelAlgorithmOperation();
    void SetAlgorithmParam(const AlgorithmParam &param);
private:
    static std::shared_ptr<FaceAuthCA> faceAuthCA_;
    static std::mutex mutex_;
    int32_t resultNum_ = 0;
    uint64_t cancelReqId_ = 0;
    bool isReturnFaceId_ = false;
    bool isInitFail_ = false;
    bool isCancel_ = false;
    HWExeType type_;
    AlgorithmParam param_;
    std::map<int32_t, ResultInfo> resultInfos_;
    std::map<int32_t, int32_t> errorCode_;
    std::vector<uint64_t> templateIdList_;
    std::map<uint64_t, int32_t> remainTimesMap_;
    AlgorithmOperation algorithmOperation_;
private:
    void GetAuthResult(int32_t &result);
    FIRetCode GetAuthState(int32_t &authErrorCode, FICode &code, uint64_t reqId);
    FIRetCode GetState(int32_t &resultCode, int32_t (&param)[RESULT_MAX_SIZE]);
    void ReadFile();
    int32_t SwitchAuthErrorCode(int32_t param);
    int32_t CheckIsCancel(int32_t &authErrorCode, FICode &code, uint64_t reqId);
    FIRetCode Cancel(uint64_t reqId);
    void InitErrorCode();
    void CheckFile(std::string s);
    void GetEnrollAngleResult();
    void GetEnrollSuccessResult();
    void GetEnrollHasRegistered();
    void GetOverMaxFace();
    void GetCaremaFail();
    void GetAngleTimeout();
    void GetResultFail();
    void GetResultTimeOut();
    FIRetCode DynamicInit();
    FIRetCode DynamicRelease();
    void ReadInitFile();
    void ReadReleaseFile();
    void CheckInitFile(std::string s);
    void CheckReleaseFile(std::string s);
    FIRetCode Prepare(HWExeType type);
    int getAlgorithmResult();
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // FACE_AUTH_CA_H
