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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_SERVICE_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_SERVICE_H
#include <list>
#include <mutex>
#include "event_handler.h"
#include "face_auth_stub.h"
#include "face_auth_manager.h"
#include "face_auth_event_handler.h"
#include "iface_auth.h"
#include "ipc_skeleton.h"
#include "nocopyable.h"
#include "singleton.h"
#include "system_ability.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };
class FaceAuthService : public SystemAbility, public FaceAuthStub {
public:
    DECLEAR_SYSTEM_ABILITY(FaceAuthService);
    static std::shared_ptr<FaceAuthService> GetInstance();
    FaceAuthService();
    virtual ~FaceAuthService() override;
    virtual int32_t Init() override;
    virtual int32_t Release() override;
    virtual int32_t Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback) override;
    virtual int32_t Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback) override;
    virtual int32_t CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback) override;
    virtual int32_t CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback) override;
    virtual int64_t PreEnroll() override;
    virtual int32_t PostEnroll() override;
    virtual int32_t Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback) override;
    virtual std::list<int32_t> GetEnrolledFaceIDs(const int32_t userId = 0) override;
    virtual void ResetTimeout() override;
    virtual int32_t GetRemainingNum() override;
    virtual int64_t GetRemainingTime() override;
    virtual int32_t GetAngleDim() override;

public:
    virtual void OnStart() override;
    virtual void OnStop() override;
    std::shared_ptr<FaceAuthEventHandler> GetEventHandler();

private:
    ServiceRunningState serviceRunningState_ = ServiceRunningState::STATE_NOT_START;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<FaceAuthEventHandler> handler_;
    DISALLOW_COPY_AND_MOVE(FaceAuthService);
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthService> instance_;
    bool HasPermission(std::string permissionName);
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_SERVICE_H
