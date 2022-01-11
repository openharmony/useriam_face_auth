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

#ifndef FACEAUTH_SERVICES_INCLUDE_IFACE_AUTH_ALGO_IMPL_H
#define FACEAUTH_SERVICES_INCLUDE_IFACE_AUTH_ALGO_IMPL_H

#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class IFaceAuthAlgoAdaptee {
public:
    virtual FIRetCode Init(int32_t imageWidth, int32_t imageHeight, const std::function<void()> &callback) = 0;
    virtual FIRetCode DynamicInit() = 0;
    virtual FIRetCode GetEnrollPoseCount(int32_t &poseCount) = 0;
    virtual FIRetCode Prepare(HWExeType type, std::unique_ptr<PrepareParam> param) = 0;
    virtual FIRetCode TranformImage(std::unique_ptr<CameraImageStream> img, int32_t count) = 0;
    virtual FIRetCode GetResult(int32_t &resultCode, int32_t param[RESULT_MAX_SIZE]) = 0;
    virtual FIRetCode Cancel() = 0;
    virtual FIRetCode Reset(uint32_t errorCode) = 0;
    virtual FIRetCode ModelUpgrade() = 0;
    virtual FIRetCode DeleteFace(uint32_t faceId) = 0;
    virtual FIRetCode DeInit() = 0;
    virtual FIRetCode SendCommand(int32_t cmd, const uint8_t &params, uint32_t paramsSize) = 0;
    virtual FIRetCode DynamicRelease() = 0;
    virtual FIRetCode CheckNeedUpgradeFeature(bool &needUpgrade) = 0;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_IFACE_AUTH_ALGO_IMPL_H