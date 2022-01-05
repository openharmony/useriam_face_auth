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

#include "on_faceauth_stub.h"
#include "faceauth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
using namespace std;

OnFaceAuthStub::OnFaceAuthStub()
{
    FACEAUTH_LABEL_LOGI("construct");
}

OnFaceAuthStub::~OnFaceAuthStub()
{
    FACEAUTH_LABEL_LOGI("de-construct");
}

int OnFaceAuthStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply,
    OHOS::MessageOption &option)
{
    FACEAUTH_LABEL_LOGI("called");
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != OnFaceAuth::GetDescriptor()) {
        FACEAUTH_LABEL_LOGE("get unexpected descriptor %{public}s", Str16ToStr8(descriptor).c_str());
        return INVALID_DATA;
    }
    int msgCode = static_cast<int>(code);
    switch (msgCode) {
        case OnFaceAuth::ON_CALLBACK_EVENT: {
            uint64_t reqId = data.ReadInt64();
            int32_t type = data.ReadInt32();
            int32_t code = data.ReadInt32();
            int32_t errorCode = data.ReadInt32();
            Bundle bundle;
            data.ReadUInt8Vector(&bundle.authtoken);

            FACEAUTH_LABEL_LOGI("reqId = xxxx%04llu, type = %{public}d, code = %{public}d, errorCode = "
                                "%{public}d",
                reqId, type, code, errorCode);
            OnCallbackEvent(reqId, type, code, errorCode, bundle);
            break;
        }
        case OnFaceAuth::ON_DEATH_RECIPIENT: {
            OnDeathRecipient();
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
