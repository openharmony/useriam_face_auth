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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CAMERA_BUFFER_LISTENER_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CAMERA_BUFFER_LISTENER_H

#include "surface.h"
#include "surface_buffer.h"
#include "input/camera_input.h"
#include "input/camera_manager.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthCameraBufferListener : public IBufferConsumerListener {
public:
    int32_t SendCameraImage(sptr<SurfaceBuffer> buffer, int64_t timestamp);
    void OnBufferAvailable() override;
    sptr<Surface> cameraBuffer_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CAMERA_BUFFER_LISTENER_H