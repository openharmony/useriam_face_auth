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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_DISPATCHER_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_DISPATCHER_H

#include <string>
#include <map>
#include <mutex>
#include <functional>
#include "faceauth_log_wrapper.h"
#include "face_auth_algo_adapter.h"
#include "face_auth_algo_impl.h"
#include "iface_auth.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthAlgoDispatcher {
public:
    static std::shared_ptr<FaceAuthAlgoDispatcher> GetInstance();
    FaceAuthAlgoDispatcher();
    virtual ~FaceAuthAlgoDispatcher();
    FIRetCode Init(std::string bundleName);
    FIRetCode GetEnrollPoseCount(int32_t &postCount);
    FIRetCode Prepare(FaceAuth::HWExeType type, std::unique_ptr<PrepareParam> param);
    FIRetCode VerifyAuthToken(std::unique_ptr<uint8_t[]> authToken, uint32_t length, int32_t type);
    FIRetCode TransformImage(std::unique_ptr<CameraImageStream> img, int32_t count);
    FIRetCode GetResult(int32_t &resultCode, int32_t param[RESULT_MAX_SIZE]);
    FIRetCode Reset(uint32_t errorCode);
    FIRetCode PostEnroll(int32_t type);
    FIRetCode Release(std::string bundleName);
    FIRetCode SetChallenge(int64_t challenge);
    FIRetCode Cancel(uint64_t reqId);
    FIRetCode GetAuthResult(int32_t &authErrorCode, FICode &code, uint64_t reqId);
    FIRetCode GetEnrollResult(int32_t &authErrorCode, FICode &code, uint64_t reqId);
    int32_t GetAuthToken(std::unique_ptr<uint8_t[]> &authToken, uint32_t &len);
    int32_t SetActiveGroup(const uint32_t uid, std::list<int32_t> &faceInfolist);
    bool IsInited();
    int32_t DeleteFace(int32_t faceId);
    FIRetCode PreEnroll(int64_t &challenge, int32_t type);

private:
    AlgoResult IsNeedAlgoLoad(std::string bundleName);
    AlgoResult IsNeedAlgoRelease(std::string bundleName);
    int32_t GetResultFromFIRetCode(FIRetCode code);
    std::map<std::string, int32_t> bundleNameList_;
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthAlgoDispatcher> instance_;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_ALGO_DISPATCHER_H
