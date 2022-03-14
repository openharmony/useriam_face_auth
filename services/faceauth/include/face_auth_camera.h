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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CAMERA_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CAMERA_H

#include <mutex>
#include "input/camera_input.h"
#include "input/camera_manager.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthCamera {
public:
    static std::shared_ptr<FaceAuthCamera> GetInstance();
    FaceAuthCamera();
    virtual ~FaceAuthCamera();
    int32_t OpenCamera(sptr<IBufferProducer> producer);
    void CloseCamera();
    void SetZoomRatio(float zoom);
    void SetFlashMode(camera_flash_mode_enum_t flash);
    void SetFocusMode(camera_af_mode_t focus);
    void SetExposureMode(camera_ae_mode_t exposure);

private:
    int32_t CreateCamera(sptr<IBufferProducer> producer);
    int32_t PrepareCamera(sptr<IBufferProducer> producer);
    int32_t CreateDisplayPreviewOutput(sptr<CameraStandard::CameraManager> &camManagerObj,
        sptr<IBufferProducer> producer);
    int32_t Start();
    void Stop();
    void Release();
    sptr<CameraStandard::CaptureOutput> CreatePreviewOutput(sptr<CameraStandard::CameraManager> &camManagerObj);
    sptr<CameraStandard::CaptureOutput> CreateTempPreviewOutput(sptr<CameraStandard::CameraManager> &camManagerObj);
    sptr<CameraStandard::CaptureInput> camInput_;
    sptr<CameraStandard::CaptureSession> capSession_;
    sptr<CameraStandard::CaptureOutput> previewOutput_;
    sptr<CameraStandard::CaptureOutput> disPlayPreviewOutput_;
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthCamera> instance_;
    bool isDisplay_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_CAMERA_H
