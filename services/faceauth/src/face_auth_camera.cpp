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

#include "face_auth_camera.h"
#include "face_auth_log_wrapper.h"
#include "face_auth_camera_buffer_listener.h"
#include "output/video_output.h"
#include "surface.h"
#include "surface_buffer.h"
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthCamera::mutex_;
std::shared_ptr<FaceAuthCamera> FaceAuthCamera::instance_ = nullptr;
std::shared_ptr<FaceAuthCamera> FaceAuthCamera::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthCamera>();
        }
    }
    return instance_;
}

FaceAuthCamera::FaceAuthCamera()
    : camInput_(nullptr), capSession_(nullptr), previewOutput_(nullptr), disPlayPreviewOutput_(nullptr),
    isDisplay_(false)
{}

FaceAuthCamera::~FaceAuthCamera()
{}

sptr<CameraStandard::CaptureOutput> FaceAuthCamera::CreatePreviewOutput(
    sptr<CameraStandard::CameraManager> &camManagerObj)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "start");
    sptr<Surface> previewBuffer = Surface::CreateSurfaceAsConsumer();
    if (previewBuffer == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "previewBuffer is nullptr");
        return nullptr;
    }
    previewBuffer->SetDefaultWidthAndHeight(PREVIEW_DEFAULT_WIDTH, PREVIEW_DEFAULT_HEIGHT);
    previewBuffer->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                               std::to_string(OHOS_CAMERA_FORMAT_YCRCB_420_SP));
    sptr<FaceAuthCameraBufferListener> listener = new FaceAuthCameraBufferListener();
    listener->cameraBuffer_ = previewBuffer;
    previewBuffer->RegisterConsumerListener((sptr<IBufferConsumerListener> &) listener);
    sptr<CameraStandard::CaptureOutput> previewOutput = camManagerObj->CreatePreviewOutput(previewBuffer);
    return previewOutput;
}

int32_t FaceAuthCamera::CreateCamera(sptr<IBufferProducer> producer)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "start");
    sptr<CameraStandard::CameraManager> camManagerObj = OHOS::CameraStandard::CameraManager::GetInstance();
    if (camManagerObj == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Get Camera Manager Failed");
        return FA_RET_ERROR;
    }
    std::vector<sptr<CameraStandard::CameraInfo>> cameraObjList;
    cameraObjList = camManagerObj->GetCameras();
    FACEAUTH_HILOGI(MODULE_SERVICE, "camera list length is %{public}zu", cameraObjList.size());
    if (cameraObjList.empty()) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "camera list is empty");
        return FA_RET_ERROR;
    }
    sptr<CameraStandard::CameraInfo> camera = nullptr;
    if (cameraObjList.size() > 0) {
        for (unsigned int i = 0; i < cameraObjList.size(); i++) {
            if (cameraObjList[i]->GetPosition() == OHOS_CAMERA_POSITION_FRONT) {
                camera = cameraObjList[i];
            }
        }
    }
    if (camera == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "front facing camera is not found");
        return FA_RET_ERROR;
    }
    camInput_ = camManagerObj->CreateCameraInput(camera);
    if (camInput_ == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Create Camera Input Failed");
        return FA_RET_ERROR;
    }
    int32_t intResult = PrepareCamera(producer);
    if (intResult != 0) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Prepare Camera Failed");
        return FA_RET_ERROR;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "end");
    return FA_RET_OK;
}

int32_t FaceAuthCamera::PrepareCamera(sptr<IBufferProducer> producer)
{
    int32_t intResult = FA_RET_OK;
    FACEAUTH_HILOGI(MODULE_SERVICE, "start");
    sptr<CameraStandard::CameraManager> camManagerObj = CameraStandard::CameraManager::GetInstance();
    if (camManagerObj == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "camManagerObj is null");
        return FA_RET_ERROR;
    }

    capSession_ = camManagerObj->CreateCaptureSession();
    if ((capSession_ == nullptr) || (camInput_ == nullptr)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create capture session failed");
        return FA_RET_ERROR;
    }
    capSession_->BeginConfig();

    intResult = capSession_->AddInput(camInput_);
    if (intResult != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "AddInput Failed");
        return FA_RET_ERROR;
    }
    previewOutput_ = CreatePreviewOutput(camManagerObj);
    if (previewOutput_ == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Failed to create PreviewOutput");
        return FA_RET_ERROR;
    }
    intResult = capSession_->AddOutput(previewOutput_);
    if (intResult != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Failed to Add Preview Output");
        return FA_RET_ERROR;
    }

    if (isDisplay_) {
        if (CreateDisplayPreviewOutput(camManagerObj, producer) != FA_RET_OK) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "Failed to create display preview output");
            return FA_RET_ERROR;
        }
        FACEAUTH_HILOGE(MODULE_SERVICE, "CreateDisplayPreviewOutput ok");
    }
    intResult = capSession_->CommitConfig();
    if (intResult != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Failed to Commit config");
        return FA_RET_ERROR;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "end");
    return FA_RET_OK;
}

int32_t FaceAuthCamera::OpenCamera(sptr<IBufferProducer> producer)
{
    if (producer != nullptr) {
        isDisplay_ = true;
    }
    if (CreateCamera(producer) != FA_RET_OK) {
        return FA_RET_ERROR;
    }
    return Start();
}

void FaceAuthCamera::CloseCamera()
{
    Stop();
    Release();
    return;
}

int32_t FaceAuthCamera::Start()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "start");
    if (capSession_ == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "capSession_ is nullptr");
        return FA_RET_ERROR;
    }
    int32_t intResult = capSession_->Start();
    if (intResult != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Start capture session failed");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

void FaceAuthCamera::Stop()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "start");
    if (capSession_ != nullptr) {
        capSession_->Stop();
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "end");
    return;
}

void FaceAuthCamera::Release()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "start");
    if (camInput_ != nullptr) {
        camInput_->Release();
        camInput_ = nullptr;
    }
    if (previewOutput_ != nullptr) {
        ((sptr<CameraStandard::PreviewOutput> &) previewOutput_)->Release();
        previewOutput_ = nullptr;
    }
    if (isDisplay_) {
        if (disPlayPreviewOutput_ != nullptr) {
            ((sptr<CameraStandard::PreviewOutput> &) disPlayPreviewOutput_)->Release();
            disPlayPreviewOutput_ = nullptr;
        }
        isDisplay_ = false;
    }
    if (capSession_ != nullptr) {
        capSession_->Release();
        capSession_ = nullptr;
    }
    if (instance_ != nullptr) {
        instance_.reset();
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "end");
    return;
}

int32_t FaceAuthCamera::CreateDisplayPreviewOutput(sptr<CameraStandard::CameraManager> &camManagerObj,
    sptr<IBufferProducer> producer)
{
    int32_t intResult = FA_RET_OK;
    FACEAUTH_HILOGI(MODULE_SERVICE, "Use UI's producer");
    disPlayPreviewOutput_ = camManagerObj->CreatePreviewOutput(producer, OHOS_CAMERA_FORMAT_YCRCB_420_SP);
    if (disPlayPreviewOutput_ == nullptr) {
        isDisplay_ = false;
        FACEAUTH_HILOGE(MODULE_SERVICE, "Failed to create PreviewOutput for UI");
        return FA_RET_ERROR;
    }
    intResult = capSession_->AddOutput(disPlayPreviewOutput_);
    if (intResult != FA_RET_OK) {
        isDisplay_ = false;
        FACEAUTH_HILOGE(MODULE_SERVICE, "Failed to Add PreviewOutput For UI");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

void FaceAuthCamera::SetZoomRatio(float zoom)
{
    if (camInput_ != nullptr) {
        ((sptr<CameraStandard::CameraInput> &) camInput_)->LockForControl();
        ((sptr<CameraStandard::CameraInput> &) camInput_)->SetZoomRatio(zoom);
        ((sptr<CameraStandard::CameraInput> &) camInput_)->UnlockForControl();
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "camInput is null, failed to set zoom ratio");
    }
    return;
}

void FaceAuthCamera::SetFlashMode(camera_flash_mode_enum_t flash)
{
    if (camInput_ != nullptr) {
        ((sptr<CameraStandard::CameraInput> &) camInput_)->LockForControl();
        ((sptr<CameraStandard::CameraInput> &) camInput_)->SetFlashMode(flash);
        ((sptr<CameraStandard::CameraInput> &) camInput_)->UnlockForControl();
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "camInput is null, failed to set flash mode");
    }
    return;
}

void FaceAuthCamera::SetFocusMode(camera_af_mode_t focus)
{
    if (camInput_ != nullptr) {
        ((sptr<CameraStandard::CameraInput> &) camInput_)->LockForControl();
        ((sptr<CameraStandard::CameraInput> &) camInput_)->SetFocusMode(focus);
        ((sptr<CameraStandard::CameraInput> &) camInput_)->UnlockForControl();
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "camInput is null, failed to set focus mode");
    }
    return;
}

void FaceAuthCamera::SetExposureMode(camera_ae_mode_t exposure)
{
    if (camInput_ != nullptr) {
        ((sptr<CameraStandard::CameraInput> &) camInput_)->LockForControl();
        ((sptr<CameraStandard::CameraInput> &) camInput_)->SetExposureMode(exposure);
        ((sptr<CameraStandard::CameraInput> &) camInput_)->UnlockForControl();
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "camInput is null, failed to set exposure mode");
    }
    return;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
