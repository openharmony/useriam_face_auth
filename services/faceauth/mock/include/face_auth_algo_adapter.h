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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_ADAPTER_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_ADAPTER_H

#include <mutex>
#include <vector>
#include <map>
#include "iface_auth_algo_target.h"
#include "iface_auth.h"
#include "mock_common.h"
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthAlgoAdapter : public IFaceAuthAlgoTarget {
public:
    static std::shared_ptr<FaceAuthAlgoAdapter> GetInstance();
    FaceAuthAlgoAdapter();
    virtual ~FaceAuthAlgoAdapter();
    FIRetCode DynamicInit();
    FIRetCode GetEnrollPoseCount(int32_t &postCount);
    FIRetCode Prepare(FaceAuth::HWExeType type, std::unique_ptr<PrepareParam> param);
    FIRetCode TransformImage(std::unique_ptr<CameraImageStream> img, int32_t count);
    FIRetCode GetResult(int32_t &resultCode, int32_t param[]);
    FIRetCode Cancel(uint64_t reqId);
    FIRetCode Reset(uint32_t errorCode);
    FIRetCode CheckNeedUpgradeFeature(bool &needUpgrade);
    FIRetCode ModelUpgrade();
    FIRetCode DynamicRelease();
    FIRetCode DeInit();
    FIRetCode SendCommand(int32_t cmd, const uint8_t &param, uint32_t paramsSize);
    FIRetCode GetAuthResult(int32_t &authErrorCode, FICode &code, uint64_t reqId);
    FIRetCode GetEnrollResult(int32_t &authErrorCode, FICode &code, uint64_t reqId);
    FIRetCode DeleteFace(int32_t faceId);

private:
    void InitErrorCode();
    int32_t SwitchAuthErrorCode(int32_t param);
    void CheckResultCode(int32_t resultCode, int32_t &authErrorCode, FICode &code,
        int32_t params[ALO_GETRESULT_PARAM_LEN]);
    int32_t CheckIsCancel(int32_t &authErrorCode, FICode &code, uint64_t reqId);
    bool isCancel_ = false;
    uint64_t cancelReqId_ = 0;
    bool isReturnFaceId_ = false;
    std::map<int32_t, int32_t> errorCode_;
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthAlgoAdapter> instance_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_ADAPTER_H
