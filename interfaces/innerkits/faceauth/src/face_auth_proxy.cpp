/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "face_auth_proxy.h"
#include "face_auth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthProxy::FaceAuthProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IFaceAuth>(object)
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
}

FaceAuthProxy::~FaceAuthProxy()
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
}

int32_t FaceAuthProxy::SetBufferProducer(sptr<IBufferProducer> &bufferProducer)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(FaceAuthProxy::GetDescriptor())) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "write descriptor failed");
        return FA_RET_ERROR;
    }
    if (bufferProducer != nullptr) {
        if (!data.WriteRemoteObject(bufferProducer->AsObject())) {
            FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to WriteRemoteObject(bufferProducer).");
            return FA_RET_ERROR;
        }
    }
    bool ret = SendRequest(FACE_AUTH_SET_BUFFER_PRODUCER, data, reply);
    if (!ret) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to send request.");
        return FA_RET_ERROR;
    }
    int32_t result = reply.ReadInt32();
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "result = %{public}d", result);
    return result;
}

bool FaceAuthProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to get remote.");
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    int32_t result = remote->SendRequest(code, data, reply, option);
    if (result != OHOS::NO_ERROR) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to SendRequest.result = %{public}d", result);
        return false;
    }
    return true;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS