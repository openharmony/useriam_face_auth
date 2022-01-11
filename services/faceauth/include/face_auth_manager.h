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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_MANAGER_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_MANAGER_H

#include <mutex>
#include "ability_manager_interface.h"
#include "bundle_mgr_interface.h"
#include "face_auth_defines.h"
#include "face_auth_req.h"
#include "face_auth_event_handler.h"
#include "face_auth_algo_impl.h"
#include "if_system_ability_manager.h"
#include "iface_auth.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthManager : public std::enable_shared_from_this<FaceAuthManager> {
public:
    static std::shared_ptr<FaceAuthManager> GetInstance();
    FaceAuthManager();
    virtual ~FaceAuthManager();
    int32_t Init();
    int32_t Release();
    int32_t Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback);
    void HandleCallEnroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback);
    void HandleCallAuthenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback);
    void HandleCallRemove(const RemoveParam &param, const sptr<OnFaceAuth> &callback);
    int32_t CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback);
    int32_t CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback);
    int32_t Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback);
    inline void SetEventHandler(const std::shared_ptr<FaceAuthEventHandler> &handler)
    {
        eventHandler_ = handler;
    }
    int32_t GetRemainingNum();
    long GetRemainingTime();
    int32_t SendCameraImage(std::unique_ptr<CameraImageStream> img);
    void ResetTimeout();
    std::list<int32_t> GetEnrolledFaceIDs(const int32_t userId = 0);
    int32_t PreEnroll(int64_t &challenge, int32_t type);
    int32_t GetAngleDim();
    int32_t Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback);
    int32_t PostEnroll(int32_t type);
    void SendCallback(const FIResultType type, const CallBackParam param, const sptr<OnFaceAuth> &callback);

private:
    int32_t CheckEnrollParam(const EnrollParam &param, const sptr<OnFaceAuth> &callback);
    int32_t CheckAuthParam(const AuthParam &param, const sptr<OnFaceAuth> &callback);
    void StartTimer();
    int32_t AddLocalAuth(const AuthParam &param, const sptr<OnFaceAuth> &callback);
    int32_t AddCoAuth(const AuthParam &param, const sptr<OnFaceAuth> &callback);
    void ExecuteAuthEvent(const uint64_t reqId, const int32_t flags, const sptr<OnFaceAuth> &callback);
    void SendAuthCallbackByChallenge(CallBackParam cbParam, const sptr<OnFaceAuth> &callback);
    void ExecuteEnrollEvent(
        uint64_t reqId, int32_t faceId, std::vector<uint8_t> token, const sptr<OnFaceAuth> &callback);
    void ExecuteRemoveEvent(const RemoveParam &param, const sptr<OnFaceAuth> &callback);
    void Prepare(HWExeType type);
    void SetChallenge(int64_t challenge);
    int32_t VerifyAuthToken(std::unique_ptr<uint8_t[]> authToken, uint32_t length, int type);
    bool CheckFaceIDValidity(int32_t faceId);
    uint32_t SwitchFaceErrorCode(int32_t authErrorCode);
    bool CheckLockOutMode(const uint64_t reqId, const sptr<OnFaceAuth> &callback);
    void HandleExceptionCallback(
        const FIResultType type, CallBackParam cbParam, const int32_t resetValue, const sptr<OnFaceAuth> &callback);
    FaceReqType CreateAuthReqInfo(uint64_t reqId, uint32_t flags);
    uint32_t GenerateEventId();
    std::mutex saMutex_;
    std::shared_ptr<FaceAuthEventHandler> eventHandler_;
    OHOS::sptr<ISystemAbilityManager> saMgr_;
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthManager> instance_;
    int64_t challenge_ = 0;
    sptr<AppExecFwk::IBundleMgr> iBundleMgr_ = nullptr;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_MANAGER_H
