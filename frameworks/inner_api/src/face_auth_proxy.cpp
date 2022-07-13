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

#include <cstdint>

#include "ibuffer_producer.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "message_option.h"
#include "message_parcel.h"
#include "refbase.h"

#include "iam_logger.h"

#include "face_auth_defines.h"
#include "iface_auth.h"

#define LOG_LABEL UserIAM::Common::LABEL_FACE_AUTH_SDK

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthProxy::FaceAuthProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IFaceAuth>(object)
{
}

int32_t FaceAuthProxy::SetBufferProducer(sptr<IBufferProducer> &bufferProducer)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(FaceAuthProxy::GetDescriptor())) {
        IAM_LOGE("write descriptor failed");
        return FACEAUTH_ERROR;
    }
    if (bufferProducer != nullptr) {
        if (!data.WriteRemoteObject(bufferProducer->AsObject())) {
            IAM_LOGE("failed to WriteRemoteObject(bufferProducer).");
            return FACEAUTH_ERROR;
        }
    }
    bool ret = SendRequest(FACE_AUTH_SET_BUFFER_PRODUCER, data, reply);
    if (!ret) {
        IAM_LOGE("failed to send request.");
        return FACEAUTH_ERROR;
    }
    int32_t result = reply.ReadInt32();
    IAM_LOGI("result = %{public}d", result);
    return result;
}

bool FaceAuthProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    IAM_LOGI("start");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        IAM_LOGE("failed to get remote.");
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    int32_t result = remote->SendRequest(code, data, reply, option);
    if (result != OHOS::NO_ERROR) {
        IAM_LOGE("failed to SendRequest.result = %{public}d", result);
        return false;
    }
    return true;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS