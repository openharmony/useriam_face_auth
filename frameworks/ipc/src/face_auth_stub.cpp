/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "face_auth_stub.h"

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <utility>

#include "ibuffer_producer.h"
#include "ipc_object_stub.h"
#include "iremote_broker.h"
#include "message_parcel.h"
#include "refbase.h"
#include "surface.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_para2str.h"

#include "face_auth_defines.h"
#include "iface_auth.h"

#define LOG_TAG "FACE_AUTH_SDK"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
using namespace OHOS::UserIam;

// When true is passed into IRemoteStub, sa will process request serially.
FaceAuthStub::FaceAuthStub() : IRemoteStub(true)
{
    IAM_LOGI("start");
    RegisterKeyToHandle();
    // For iface_cast<IBufferProducer> to work, static variable BrokerDelegator of BufferClientProducer must be
    // initialized. Create an unused surface would include BufferClientProducer and ensure BrokerDelegator is
    // initialized.
    OHOS::Surface::CreateSurfaceAsConsumer("unused");
}

void FaceAuthStub::RegisterKeyToHandle()
{
    keyToHandle_[IFaceAuthInterfaceCode::FACE_AUTH_SET_BUFFER_PRODUCER] = &FaceAuthStub::FaceAuthSetBufferProducer;
}

int32_t FaceAuthStub::FaceAuthSetBufferProducer(MessageParcel &data, MessageParcel &reply)
{
    sptr<IBufferProducer> buffer(nullptr);
    sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
    IAM_LOGI("read remote object %{public}s", Common::GetPointerNullStateString(remoteObj).c_str());
    buffer = iface_cast<IBufferProducer>(remoteObj);
    bool remoteObjNull = (remoteObj == nullptr);
    bool bufferNull = (buffer == nullptr);
    IF_FALSE_LOGE_AND_RETURN_VAL(bufferNull == remoteObjNull, FACE_AUTH_ERROR);
    int32_t ret = SetBufferProducer(buffer);
    IAM_LOGI("SetBufferProducer ret %{public}d", ret);
    if (!reply.WriteInt32(ret)) {
        IAM_LOGE("failed to WriteInt32(ret)");
        return FACE_AUTH_ERROR;
    }
    return FACE_AUTH_SUCCESS;
}

int32_t FaceAuthStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    IAM_LOGI("start");
    if (data.ReadInterfaceToken() != FaceAuthStub::GetDescriptor()) {
        IAM_LOGE("descriptor is not matched");
        return FACE_AUTH_ERROR;
    }
    auto itFunc = keyToHandle_.find(code);
    if (itFunc == keyToHandle_.end()) {
        IAM_LOGE("key not match, send to IPCObjectStub on default");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    auto requestFunc = itFunc->second;
    IF_FALSE_LOGE_AND_RETURN_VAL(requestFunc != nullptr, FACE_AUTH_ERROR);
    return (this->*requestFunc)(data, reply);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS