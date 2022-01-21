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

#include "face_auth_camera.h"
#include <string>
#include "output/video_output.h"
#include "faceauth_log_wrapper.h"
#include "face_auth_algo_impl.h"
#include "face_auth_camera_buffer_listener.h"
#include "face_auth_defines.h"
#include "surface.h"
#include "surface_buffer.h"

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
    : camInput_(nullptr),
      capSession_(nullptr),
      previewOutput_(nullptr),
      disPlayPreviewOutput_(nullptr),
      isDisplay_(false)
{}

FaceAuthCamera::~FaceAuthCamera()
{}

sptr<CameraStandard::CaptureOutput> FaceAuthCamera::CreatePreviewOutput(
    sptr<CameraStandard::CameraManager> &camManagerObj)
{
    FACEAUTH_LABEL_LOGI("CreatePreviewOutput.");
    sptr<Surface> previewBuffer = Surface::CreateSurfaceAsConsumer();
    previewBuffer->SetDefaultWidthAndHeight(PREVIEW_DEFAULT_WIDTH, PREVIEW_DEFAULT_HEIGHT);
    previewBuffer->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                               std::to_string(OHOS_CAMERA_FORMAT_YCRCB_420_SP));
    sptr<FaceAuthCameraBufferListener> listener = new FaceAuthCameraBufferListener();
    listener->cameraBuffer_ = previewBuffer;
    previewBuffer->RegisterConsumerListener((sptr<IBufferConsumerListener> &) listener);
    sptr<CameraStandard::CaptureOutput> previewOutput = camManagerObj->CreatePreviewOutput(previewBuffer);
    return previewOutput;
}

int32_t FaceAuthCamera::CreateCamera(int32_t previewId)
{
    FACEAUTH_LABEL_LOGI("Create Camera start.");
    sptr<CameraStandard::CameraManager> camManagerObj = OHOS::CameraStandard::CameraManager::GetInstance();

    if (camManagerObj == nullptr) {
        FACEAUTH_LABEL_LOGE("Get Camera Manager Failed");
        return FA_RET_ERROR;
    }
    std::vector<sptr<CameraStandard::CameraInfo>> cameraObjList;
    cameraObjList = camManagerObj->GetCameras();
    FACEAUTH_LABEL_LOGI("GetCameras:%{public}d", cameraObjList.size());
    if (cameraObjList.empty()) {
        FACEAUTH_LABEL_LOGE("Get Cameras Failed");
        return FA_RET_ERROR;
    }
    sptr<CameraStandard::CameraInfo> camera = cameraObjList[0];
    if (cameraObjList.size() > 1) {
        for (unsigned int i = 0; i < cameraObjList.size(); i++) {
            if (cameraObjList[i]->GetPosition() == OHOS_CAMERA_POSITION_FRONT) {
                camera = cameraObjList[i];
            }
        }
    }
    camInput_ = camManagerObj->CreateCameraInput(camera);
    if (camInput_ == nullptr) {
        FACEAUTH_LABEL_LOGE("Create Camera Input Failed");
        return FA_RET_ERROR;
    }
    int32_t intResult = PrepareCamera(previewId);
    if (intResult != 0) {
        FACEAUTH_LABEL_LOGE("Prepare Camera Failed");
        return FA_RET_ERROR;
    }
    FACEAUTH_LABEL_LOGI("Create Camera end.");
    return FA_RET_OK;
}

int32_t FaceAuthCamera::PrepareCamera(int32_t previewId)
{
    int32_t intResult = FA_RET_OK;
    FACEAUTH_LABEL_LOGI("Prepare Camera start.");
    sptr<CameraStandard::CameraManager> camManagerObj = CameraStandard::CameraManager::GetInstance();
    capSession_ = camManagerObj->CreateCaptureSession();
    if ((capSession_ == nullptr) || (camInput_ == nullptr)) {
        FACEAUTH_LABEL_LOGE("Create was not Proper!");
        return FA_RET_ERROR;
    }
    capSession_->BeginConfig();

    intResult = capSession_->AddInput(camInput_);
    if (intResult != FA_RET_OK) {
        FACEAUTH_LABEL_LOGE("AddInput Failed!");
        return FA_RET_ERROR;
    }
    FACEAUTH_LABEL_LOGI("CreatePreviewOutput");
    previewOutput_ = CreatePreviewOutput(camManagerObj);
    if (previewOutput_ == nullptr) {
        FACEAUTH_LABEL_LOGE("Failed to create PreviewOutput");
        return FA_RET_ERROR;
    }
    intResult = capSession_->AddOutput(previewOutput_);
    if (intResult != FA_RET_OK) {
        FACEAUTH_LABEL_LOGE("Failed to Add Preview Output");
        return FA_RET_ERROR;
    }
    if (isDisplay_) {
        if (CreateDisplayPreviewOutput(camManagerObj, previewId) != FA_RET_OK) {
            FACEAUTH_LABEL_LOGE("Switch PreviewOutput Failed!");
            return FA_RET_ERROR;
        }
    }
    intResult = capSession_->CommitConfig();
    if (intResult != FA_RET_OK) {
        FACEAUTH_LABEL_LOGE("Failed to Commit config");
        return FA_RET_ERROR;
    }
    FACEAUTH_LABEL_LOGI("Prepare Camera end.");
    return FA_RET_OK;
}

int32_t FaceAuthCamera::OpenCamera(int32_t previewId)
{
    if (previewId != 0) {
        isDisplay_ = true;
    }
    if (CreateCamera(previewId) != FA_RET_OK) {
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
    FACEAUTH_LABEL_LOGI("FaceAuthCamera::Start.");
    int32_t intResult = capSession_->Start();
    if (intResult != FA_RET_OK) {
        FACEAUTH_LABEL_LOGE("Camera::Start Capture Session Failed");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

void FaceAuthCamera::Stop()
{
    FACEAUTH_LABEL_LOGI("FaceAuthCamera::Stop.");
    if (capSession_ != nullptr) {
        capSession_->Stop();
    }
    FACEAUTH_LABEL_LOGI("FaceAuthCamera::Stop End.");
    return;
}

void FaceAuthCamera::Release()
{
    FACEAUTH_LABEL_LOGI("FaceAuthCamera::Release Start.");
    camInput_ = nullptr;
    if (previewOutput_ != nullptr) {
        ((sptr<CameraStandard::PreviewOutput> &) previewOutput_)->Release();
        FACEAUTH_LABEL_LOGI("FaceAuthCamera::previewOutput_ Release End.");
        previewOutput_ = nullptr;
    }
    if (isDisplay_) {
        if (disPlayPreviewOutput_ != nullptr) {
            ((sptr<CameraStandard::PreviewOutput> &) disPlayPreviewOutput_)->Release();
            FACEAUTH_LABEL_LOGI("FaceAuthCamera::switchPreviewOutput_ Release End.");
            disPlayPreviewOutput_ = nullptr;
        }
        isDisplay_ = false;
    }
    if (capSession_ != nullptr) {
        capSession_->Release();
        capSession_ = nullptr;
    }
    FACEAUTH_LABEL_LOGI("FaceAuthCamera::Release End.");
    return;
}

sptr<Surface> FaceAuthCamera::GetXComponentSurfaceById(std::string id)
{
    return nullptr;
}

int32_t FaceAuthCamera::CreateDisplayPreviewOutput(sptr<CameraStandard::CameraManager> &camManagerObj,
    int32_t previewId)
{
    int32_t intResult = FA_RET_OK;
    FACEAUTH_LABEL_LOGI("Use UI's PreviewBuffer");
    std::string id = std::to_string(previewId);
    FACEAUTH_LABEL_LOGI("GetXComponentSurfaceById Id:%{public}s", id.c_str());
    sptr<Surface> previewBuffer = GetXComponentSurfaceById(id);
    if (previewBuffer == nullptr) {
        isDisplay_ = false;
        FACEAUTH_LABEL_LOGE("Failed to GetXComponentSurfaceById");
        return FA_RET_OK;
    }
    sptr<CameraStandard::CaptureOutput> disPlayPreviewOutput_ = camManagerObj->CreatePreviewOutput(previewBuffer);
    if (disPlayPreviewOutput_ == nullptr) {
        isDisplay_ = false;
        FACEAUTH_LABEL_LOGE("Failed to create PreviewOutput for UI");
        return FA_RET_ERROR;
    }
    intResult = capSession_->AddOutput(disPlayPreviewOutput_);
    if (intResult != FA_RET_OK) {
        isDisplay_ = false;
        FACEAUTH_LABEL_LOGE("Failed to Add PreviewOutput For UI");
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
        FACEAUTH_LABEL_LOGE("camInput is null,failed to set zoom ratio.");
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
        FACEAUTH_LABEL_LOGE("camInput is null,failed to set flash mode.");
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
        FACEAUTH_LABEL_LOGE("camInput is null,failed to set focus mode.");
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
        FACEAUTH_LABEL_LOGE("camInput is null,failed to set exposure mode.");
    }
    return;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
