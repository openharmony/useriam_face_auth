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

#include "face_auth_camera_buffer_listener.h"
#include <securec.h>
#include "faceauth_log_wrapper.h"
#include "face_auth_algo_impl.h"
#include "face_auth_manager.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
int32_t FaceAuthCameraBufferListener::SendCameraImage(OHOS::sptr<OHOS::SurfaceBuffer> buffer, int64_t timestamp)
{
    CameraImageStream ImageStream = {};
    ImageStream.secureType = 0; /* 0: normal 1: secure */
    ImageStream.streamType = 1;
    ImageStream.timestamp = timestamp;
    ImageStream.irStatus = 1;
    ImageStream.bufferType = 0; /* 0 dynamic memory, 1 static memory, 2 remote image */
    ImageStream.size = buffer->GetSize();
    ImageStream.isExposureValid = 0; /* AE status: 0 invalid, 1 valid */
    BufferHandle *bufferHandle = buffer->GetBufferHandle();
    ImageStream.width = bufferHandle->width;
    ImageStream.height = bufferHandle->height;
    ImageStream.format = bufferHandle->format;
    ImageStream.stride = bufferHandle->stride;
    ImageStream.data.image = buffer->GetVirAddr();
    std::unique_ptr<CameraImageStream> ImageStream_ptr = std::make_unique<CameraImageStream>();
    ImageStream_ptr.reset(&ImageStream);
    // Send CameraImageStream* to FaceAuthManager.
    FaceAuthManager::GetInstance()->SendCameraImage(std::move(ImageStream_ptr));
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
        FACEAUTH_LABEL_LOGI("AcquireBuffer failed!");
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS