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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_REQ_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_REQ_H
#include <map>
#include <mutex>
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthReq {
public:
    FaceAuthReq();
    virtual ~FaceAuthReq();
    static std::shared_ptr<FaceAuthReq> GetInstance();
    bool IsReqNumReachedMax(FaceOperateType type);
    void AddReqInfo(FaceReqType reqType, FaceInfo reqInfo);
    void RemoveRequireInfo(FaceReqType reqType);
    void RemoveAllRequireInfo();
    FaceOperateType GetOperateType(uint32_t eventId);
    uint32_t GetEventId(FaceReqType reqType);
    bool SetCancelFlagSuccess(FaceReqType reqType, int32_t uId);
    bool isCanceled(uint32_t eventId, int32_t uId);
    bool FindLocalAuth();
    void PrintReqInfoList();
private:
    std::map<FaceReqType, FaceInfo> reqInfoList_;
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthReq> instance_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_REQ_H
