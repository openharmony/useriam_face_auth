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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_IMPL_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_IMPL_H

#include <map>
#include <mutex>
#include <functional>
#include <stdint.h>
#include <vector>
#include "face_auth_defines.h"
#include "mock_common.h"
#include "iface_auth_algo_adaptee.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthAlgoImpl : public IFaceAuthAlgoAdaptee {
public:
    static std::shared_ptr<FaceAuthAlgoImpl> GetInstance();
    FaceAuthAlgoImpl();
    virtual ~FaceAuthAlgoImpl();
    FIRetCode Init(int32_t imageWidth, int32_t imageHeight, const std::function<void()> &callback);
    FIRetCode DynamicInit();
    FIRetCode GetEnrollPoseCount(int32_t &poseCount);
    FIRetCode Prepare(HWExeType type, std::unique_ptr<PrepareParam> param);
    FIRetCode TranformImage(std::unique_ptr<CameraImageStream> img, int32_t count);
    FIRetCode GetResult(int32_t &resultCode, int32_t param[RESULT_MAX_SIZE]);
    FIRetCode Cancel();
    FIRetCode Reset(uint32_t errorCode);
    FIRetCode ModelUpgrade();
    FIRetCode DeleteFace(uint32_t faceId);
    FIRetCode DeInit();
    FIRetCode SendCommand(int32_t cmd, const uint8_t &params, uint32_t paramsSize);
    FIRetCode DynamicRelease();
    FIRetCode CheckNeedUpgradeFeature(bool &needUpgrade);

private:
    void ReadFile();
    void GetEnrollAngleResult();
    void GetOverMaxFace();
    void GetCaremaFail();
    void GetEnrollHasRegistered();
    void GetEnrollSuccessResult();
    void GetAngleTimeout();
    void GetResultFail();
    void GetResultTimeOut();
    int32_t SwitchAuthErrorCode(int32_t param);
    void CheckFile(std::string s);
    void CheckInitFile(std::string s);
    void CheckReleaseFile(std::string s);
    void ReadInitFile();
    void ReadReleaseFile();
    std::map<int32_t, ResultInfo> resultInfos_;
    int32_t resultNum_ = 0;
    bool isReturnFaceId_ = false;
    HWExeType type_;
    bool isInitFail_;
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthAlgoImpl> instance_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_IMPL_H
