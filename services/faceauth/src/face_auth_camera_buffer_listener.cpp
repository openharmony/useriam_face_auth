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

#include "face_auth_camera_buffer_listener.h"
#include <securec.h>
#include "face_auth_log_wrapper.h"
#include "face_auth_ca.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
int32_t FaceAuthCameraBufferListener::SendCameraImage(OHOS::sptr<OHOS::SurfaceBuffer> buffer, int64_t timestamp)
{
    CameraImage image = {};
    image.image = static_cast<uint8_t*>(buffer->GetVirAddr());
    image.imageSize = buffer->GetSize();
    BufferHandle *bufferHandle = buffer->GetBufferHandle();
    image.width = bufferHandle->width;
    image.height = bufferHandle->height;
    image.stride = bufferHandle->stride;
    image.timestamp = timestamp;
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA = nullptr.");
        return -1;
    }
    faceAuthCA->TransferImageToAlgorithm(image);
    return 0;
}

void FaceAuthCameraBufferListener::OnBufferAvailable()
{
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage;
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    cameraBuffer_->AcquireBuffer(buffer, flushFence, timestamp, damage);
    if (buffer != nullptr) {
        SendCameraImage(buffer, timestamp);
        cameraBuffer_->ReleaseBuffer(buffer, -1);
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "AcquireBuffer failed!");
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS