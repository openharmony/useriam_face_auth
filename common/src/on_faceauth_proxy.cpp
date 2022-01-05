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

#include "on_faceauth_proxy.h"
#include "faceauth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
using namespace std;

OnFaceAuthProxy::OnFaceAuthProxy(const OHOS::sptr<OHOS::IRemoteObject> &object) : IRemoteProxy(object)
{
    FACEAUTH_LABEL_LOGI("construct");
}

OnFaceAuthProxy::~OnFaceAuthProxy()
{
    FACEAUTH_LABEL_LOGI("de-construct");
}

void OnFaceAuthProxy::OnCallbackEvent(uint64_t reqId, int32_t type, int32_t code, int32_t errorCode, Bundle &bundle)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        FACEAUTH_LABEL_LOGE("remote service null");
        return;
    }

    FACEAUTH_LABEL_LOGI("Begin SendRequest");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(OnFaceAuth::GetDescriptor());
    if (!data.WriteInt64(reqId)) {
        FACEAUTH_LABEL_LOGE("Failed to write reqId");
        return;
    }
    if (!data.WriteInt32(type)) {
        FACEAUTH_LABEL_LOGE("Failed to write type");
        return;
    }
    if (!data.WriteInt32(code)) {
        FACEAUTH_LABEL_LOGE("Failed to write code");
        return;
    }
    if (!data.WriteInt32(errorCode)) {
        FACEAUTH_LABEL_LOGE("Failed to write errorCode");
        return;
    }
    if (!data.WriteUInt8Vector(bundle.authtoken)) {
        FACEAUTH_LABEL_LOGE("Failed to write bundle.authtoken");
        return;
    }
    int32_t ret = remote->SendRequest(OnFaceAuth::ON_CALLBACK_EVENT, data, reply, option);
    if (ret != 0) {
        FACEAUTH_LABEL_LOGE("SendRequest fail, error: %{public}d", ret);
        return;
    }
    FACEAUTH_LABEL_LOGI("SendRequest success");
}

void OnFaceAuthProxy::OnDeathRecipient()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        FACEAUTH_LABEL_LOGE("remote service null");
        return;
    }

    FACEAUTH_LABEL_LOGI("Begin SendRequest");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(OnFaceAuth::GetDescriptor());
    int32_t ret = remote->SendRequest(OnFaceAuth::ON_DEATH_RECIPIENT, data, reply, option);
    if (ret != 0) {
        FACEAUTH_LABEL_LOGE("SendRequest fail, error: %{public}d", ret);
        return;
    }
    FACEAUTH_LABEL_LOGI("SendRequest success");
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS