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

#include "face_auth_ca.h"
#include "defines.h"
#include "face_auth_func.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::shared_ptr<FaceAuthCA> FaceAuthCA::faceAuthCA_ = nullptr;
std::mutex FaceAuthCA::mutex_;
const int BUFF_MAX_LEN = 128;
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
    return 0;
}

int32_t FaceAuthCA::Close()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    return 0;
}

int32_t FaceAuthCA::LoadAlgorithm()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    return 0;
}

int32_t FaceAuthCA::ReleaseAlgorithm()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    return 0;
}

int32_t FaceAuthCA::StartAlgorithmOperation(AlgorithmOperation algorithmOperation, AlgorithmParam param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    (void)(algorithmOperation);
    SetAlgorithmParam(param);
    return 0;
}

int32_t FaceAuthCA::TransferImageToAlgorithm(CameraImage images)
{
    sleep(1);
    (void)(images);
    return 0;
}

void FaceAuthCA::GetAlgorithmState(int32_t &retCode, std::vector<uint8_t> &retCoauthMsg)
{
    retCode = 1;
    (void)(retCoauthMsg);
    return ;
}

int32_t FaceAuthCA::GetExecutorInfo(std::vector<uint8_t> &pubKey, uint32_t &esl, uint64_t &authAbility)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    if (DoGetExecutorInfo(pubKey, esl, authAbility) != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "DoGetExecutorInfo failed.");
        return -1;
    }
    return 0;
}

int32_t FaceAuthCA::FinishAlgorithmOperation(AlgorithmResult &retResult)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    int32_t authResult = 0;
    GetAuthResult(authResult);
    FACEAUTH_HILOGI(MODULE_SERVICE, "get auth result = %{public}d.", authResult);
    retResult.result = authResult;
    retResult.templateId = param_.templateId;
    GetRemainTimes(param_.templateId, retResult.remainTimes);
    Buffer *retTlv = CreateBuffer(RESULT_TLV_LEN);
    if (retTlv == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "CreateBuffer failed.");
        return -1;
    }
    ResultCode result = GenerateRetTlv(RESULT_SUCCESS, param_.scheduleId, FACE_2D, param_.templateId, retTlv);
    if (result != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GenerateRetTlv failed.");
        DestoryBuffer(retTlv);
        return -1;
    }
    result = SetResultTlv(retTlv, retResult.coauthMsg);
    if (result != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "SetResultTlv failed.");
        DestoryBuffer(retTlv);
        return -1;
    }
    DestoryBuffer(retTlv);
    return 0;
}

int32_t FaceAuthCA::DeleteTemplete(uint64_t templateId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    (void)(templateId);
    return 0;
}

int32_t FaceAuthCA::VerifyTemplateData(std::vector<uint64_t> templateIdList)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    (void)(templateIdList);
    return 0;
}

int32_t FaceAuthCA::GetFaceInfo(uint64_t templateId, FaceCredentialInfo &faceCredentialInfo)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    faceCredentialInfo.subType = FACE_2D;
    faceCredentialInfo.freezingTime = 0;
    faceCredentialInfo.remainTimes = MAX_REMAIN_TIMES;
    return 0;
}

int32_t FaceAuthCA::GetRemainTimes(uint64_t templateId, int32_t &remainingTimes)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    remainingTimes = MAX_REMAIN_TIMES;
    return 0;
}

int32_t FaceAuthCA::ResetRemainTimes(uint64_t templateId)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    return 0;
}

int32_t FaceAuthCA::CancelAlogrithmOperation()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    return 0;
}
void FaceAuthCA::SetAlgorithmParam(const AlgorithmParam &param)
{
    param_ = param;
}
// temp code start
void GetAuthResult(int32_t &result)
{
    FILE* file = nullptr;
    file = fopen("/data/useriam/auth_result.txt", "r");
    if (file == nullptr) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "open file failed.");
        return;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "fseek failed.");
        fclose(file);
        return;
    }
    if (ftell(file) < 0) {
        fseek(file, 0, SEEK_SET);
        char str[BUFF_MAX_LEN] = {0};
        fread((void *)str, sizeof(char), BUFF_MAX_LEN - 1, file);
        result = atoi(str);
    }
    if (fclose(file) != 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "fclose failed.");
    }
}
// temp code end
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
