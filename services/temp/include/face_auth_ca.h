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
#include "face_auth_defines.h"
#include "face_auth_log_wrapper.h"
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
typedef enum {
    Enroll,
    Auth,
} AlgorithmOperation;

typedef struct {
    uint64_t templateId;
    uint64_t scheduleId;
} AlgorithmParam;

typedef struct {
    uint8_t *image;
    uint32_t imageSize;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    int64_t timestamp;
} CameraImage;

typedef struct {
    int32_t result;
    int32_t remainTimes;
    uint64_t templateId;
    std::vector<uint8_t> coauthMsg; // coauth signed msg
} AlgorithmResult;

typedef struct FaceCredentialInfo {
    uint64_t subType;
    uint32_t remainTimes;
    uint32_t freezingTime;
} FaceCredentialInfo;

// temp code start
void GetAuthResult(int32_t &result);
// temp code end
class FaceAuthCA {
public:
    static std::shared_ptr<FaceAuthCA> GetInstance();
    FaceAuthCA()=default;
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
    int32_t DeleteTemplete(uint64_t templateId);
    int32_t VerifyTemplateData(std::vector<uint64_t> templateIdList);
    int32_t GetRemainTimes(uint64_t templateId, int32_t &remainingTimes);
    int32_t GetFaceInfo(uint64_t templateId, FaceCredentialInfo &faceCredentialInfo);
    int32_t ResetRemainTimes(uint64_t templateId);
    int32_t CancelAlogrithmOperation();
    void SetAlgorithmParam(const AlgorithmParam &param);
private:
    static std::shared_ptr<FaceAuthCA> faceAuthCA_;
    static std::mutex mutex_;
    AlgorithmParam param_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // FACE_AUTH_CA_H
