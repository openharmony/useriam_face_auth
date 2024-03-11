/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "face_auth_client.h"

#include <cstdint>
#include <mutex>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "if_system_ability_manager.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "nocopyable.h"
#include "refbase.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"
#include "token_setproc.h"

#include "iam_logger.h"

#include "face_auth_defines.h"
#include "iface_auth.h"

#define LOG_TAG "FACE_AUTH_SDK"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthClientImpl : public FaceAuthClient, public NoCopyable {
public:
    int32_t SetBufferProducer(sptr<IBufferProducer> &producer) override;

private:
    friend class FaceAuthClient;
    FaceAuthClientImpl() = default;
    ~FaceAuthClientImpl() override = default;
    static FaceAuthClientImpl &Instance();
    class FaceAuthClientImplDeathRecipient : public IRemoteObject::DeathRecipient, public NoCopyable {
    public:
        FaceAuthClientImplDeathRecipient() = default;
        ~FaceAuthClientImplDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };
    bool CheckSystemPermission();
    void ResetProxy(const wptr<IRemoteObject> &remote);
    sptr<IFaceAuth> GetProxy();
    sptr<IFaceAuth> proxy_ {nullptr};
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
    std::mutex mutex_;
    uint64_t tokenIdLowMask_ = 0xffffffff;
};

bool FaceAuthClientImpl::CheckSystemPermission()
{
    using namespace Security::AccessToken;
    uint64_t fullTokenId = GetSelfTokenID();
    bool checkRet = TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
    AccessTokenID tokenId = fullTokenId & tokenIdLowMask_;
    if (AccessTokenKit::GetTokenType(tokenId) == TOKEN_HAP && !checkRet) {
        return false;
    }
    return true;
}

int32_t FaceAuthClientImpl::SetBufferProducer(sptr<IBufferProducer> &producer)
{
    IAM_LOGI("start");
    if (!CheckSystemPermission()) {
        IAM_LOGE("the caller is not a system application");
        return FACE_AUTH_CHECK_SYSTEM_PERMISSION_FAILED;
    }
    sptr<IFaceAuth> proxy = GetProxy();
    if (proxy == nullptr) {
        IAM_LOGE("get faceAuthProxy fail");
        return FACE_AUTH_ERROR;
    }
    return proxy->SetBufferProducer(producer);
}

sptr<IFaceAuth> FaceAuthClientImpl::GetProxy()
{
    IAM_LOGI("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ != nullptr) {
        return proxy_;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        IAM_LOGE("failed to get systemAbilityManager.");
        return nullptr;
    }

    sptr<IRemoteObject> obj = systemAbilityManager->CheckSystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH);
    if (obj == nullptr) {
        IAM_LOGE("failed to get remoteObject.");
        return nullptr;
    }

    sptr<IRemoteObject::DeathRecipient> dr(new (std::nothrow) FaceAuthClientImplDeathRecipient());
    if ((dr == nullptr) || (obj->IsProxyObject() && !obj->AddDeathRecipient(dr))) {
        IAM_LOGE("add death recipient fail");
        return nullptr;
    }

    proxy_ = iface_cast<IFaceAuth>(obj);
    deathRecipient_ = dr;
    return proxy_;
}

void FaceAuthClientImpl::ResetProxy(const wptr<IRemoteObject> &remote)
{
    IAM_LOGI("start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ == nullptr) {
        IAM_LOGE("proxy_ is null");
        return;
    }
    auto serviceRemote = proxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        IAM_LOGI("need reset");
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        proxy_ = nullptr;
        deathRecipient_ = nullptr;
    }
    IAM_LOGI("end reset proxy");
}

void FaceAuthClientImpl::FaceAuthClientImplDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    IAM_LOGI("start");
    if (remote == nullptr) {
        IAM_LOGE("remote is nullptr");
        return;
    }
    FaceAuthClientImpl::Instance().ResetProxy(remote);
}

FaceAuthClientImpl &FaceAuthClientImpl::Instance()
{
    static FaceAuthClientImpl impl;
    return impl;
}

FaceAuthClient &FaceAuthClient::GetInstance()
{
    return FaceAuthClientImpl::Instance();
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
