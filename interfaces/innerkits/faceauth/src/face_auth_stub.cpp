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

#include "face_auth_stub.h"
#include "securec.h"
#include "face_auth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthStub::FaceAuthStub()
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
    RegisterKeyToHandle();
}

FaceAuthStub::~FaceAuthStub()
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
}

void FaceAuthStub::RegisterKeyToHandle()
{
    keyToHandle_[FACE_AUTH_SET_BUFFER_PRODUCER] = &FaceAuthStub::FaceAuthSetBufferProducer;
}

int32_t FaceAuthStub::FaceAuthSetBufferProducer(MessageParcel &data, MessageParcel &reply)
{
    sptr<IBufferProducer> buffer = nullptr;
    sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "read remote object %{public}s",
        getPointerNullString(remoteObj).c_str());
    if (remoteObj != nullptr) {
        buffer = iface_cast<OHOS::IBufferProducer>(remoteObj);
    }
    int32_t ret = SetBufferProducer(buffer);
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "SetBufferProducer ret %{public}d", ret);
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "failed to WriteInt32(ret)");
        return FA_RET_OK;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "start");
    if (data.ReadInterfaceToken() != FaceAuthStub::GetDescriptor()) {
        FACEAUTH_HILOGE(MODULE_FRAMEWORK, "descriptor is not matched");
        return FA_RET_ERROR;
    }
    auto itFunc = keyToHandle_.find(code);
    if (itFunc != keyToHandle_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    FACEAUTH_HILOGI(MODULE_FRAMEWORK, "AbilitySchedulerStub::OnRemoteRequest, default case, ignore.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS