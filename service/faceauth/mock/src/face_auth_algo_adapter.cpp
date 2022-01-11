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

#include "face_auth_algo_adapter.h"
#include <thread>
#include "faceauth_log_wrapper.h"
#include "face_auth_ca.h"
#include "face_auth_algo_impl.h"
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthAlgoAdapter::mutex_;
std::shared_ptr<FaceAuthAlgoAdapter> FaceAuthAlgoAdapter::instance_ = nullptr;
static const int32_t SLEEP_TIME = 5000;
std::shared_ptr<FaceAuthAlgoAdapter> FaceAuthAlgoAdapter::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthAlgoAdapter>();
        }
    }
    return instance_;
}

FaceAuthAlgoAdapter::FaceAuthAlgoAdapter() : isCancel_(false), cancelReqId_(0), isReturnFaceId_(false)
{
    InitErrorCode();
}

FaceAuthAlgoAdapter::~FaceAuthAlgoAdapter()
{}

FIRetCode FaceAuthAlgoAdapter::DynamicInit()
{
    return FaceAuthAlgoImpl::GetInstance()->DynamicInit();
}

FIRetCode FaceAuthAlgoAdapter::DynamicRelease()
{
    return FaceAuthAlgoImpl::GetInstance()->DynamicRelease();
}

FIRetCode FaceAuthAlgoAdapter::GetEnrollPoseCount(int32_t &postCount)
{
    return FaceAuthAlgoImpl::GetInstance()->GetEnrollPoseCount(postCount);
}

FIRetCode FaceAuthAlgoAdapter::Prepare(HWExeType type, std::unique_ptr<PrepareParam> param)
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoAdapter::Prepare start");
    if (type < HW_EXEC_TYPE_ENROOL || type > HW_EXEC_TYPE_LEARN) {
        FACEAUTH_LABEL_LOGE("Parameter check error.type is %{public}d", type);
        return FI_RC_INVALID_ARGUMENT;
    }
    if (param == nullptr) {
        FACEAUTH_LABEL_LOGE("Parameter check error.param is null");
        return FI_RC_NULLPOINTER;
    }
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoAdapter::Prepare end");
    int32_t result = FaceAuthAlgoImpl::GetInstance()->Prepare(type, std::move(param));
    if (result == 0) {
        return FI_RC_OK;
    }

    return FI_RC_ERROR;
}

FIRetCode FaceAuthAlgoAdapter::TransformImage(std::unique_ptr<CameraImageStream> img, int32_t count)
{
    if (img == nullptr) {
        FACEAUTH_LABEL_LOGE("Parameter check error.img is null");
        return FI_RC_NULLPOINTER;
    }
    if (count < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.count is %{public}d", count);
        return FI_RC_INVALID_ARGUMENT;
    }
    return FaceAuthAlgoImpl::GetInstance()->TranformImage(std::move(img), count);
}

FIRetCode FaceAuthAlgoAdapter::GetResult(int32_t &resultCode, int32_t param[])
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoAdapter::GetResult start");
    return FaceAuthAlgoImpl::GetInstance()->GetResult(resultCode, param);
}

FIRetCode FaceAuthAlgoAdapter::Cancel(uint64_t reqId)
{
    isCancel_ = true;
    cancelReqId_ = reqId;
    FaceAuthAlgoImpl::GetInstance()->Cancel();
    return FI_RC_OK;
}

FIRetCode FaceAuthAlgoAdapter::Reset(uint32_t errorCode)
{
    if (errorCode < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.errorCode is %{public}d", errorCode);
        return FI_RC_INVALID_ARGUMENT;
    }
    FaceAuthAlgoImpl::GetInstance()->Reset(errorCode);
    FACEAUTH_LABEL_LOGI("errorCode is %{public}d", errorCode);
    return FI_RC_OK;
}
FIRetCode FaceAuthAlgoAdapter::CheckNeedUpgradeFeature(bool &needUpgrade)
{
    return FaceAuthAlgoImpl::GetInstance()->CheckNeedUpgradeFeature(needUpgrade);
}

FIRetCode FaceAuthAlgoAdapter::ModelUpgrade()
{
    return FaceAuthAlgoImpl::GetInstance()->ModelUpgrade();
}

FIRetCode FaceAuthAlgoAdapter::DeleteFace(int32_t faceId)
{
    if (faceId < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.faceId is %{private}d", faceId);
        return FI_RC_INVALID_ARGUMENT;
    }
    int32_t result = FaceAuthCa::GetInstance()->DeleteTemolate(faceId);
    if (result == 0) {
        result = FaceAuthCa::GetInstance()->DeleteTemplateStep2();
        if (result == 0) {
            return FI_RC_OK;
        }
    }
    return FI_RC_ERROR;
}

FIRetCode FaceAuthAlgoAdapter::DeInit()
{
    return FaceAuthAlgoImpl::GetInstance()->DeInit();
}

FIRetCode FaceAuthAlgoAdapter::SendCommand(int32_t cmd, const uint8_t &param, uint32_t paramsSize)
{
    if (cmd < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.cmd is %{public}d", cmd);
        return FI_RC_INVALID_ARGUMENT;
    }
    if (paramsSize < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.paramsSize is %{public}d", paramsSize);
        return FI_RC_INVALID_ARGUMENT;
    }
    return FI_RC_OK;
}

void FaceAuthAlgoAdapter::InitErrorCode()
{
    errorCode_[FI_COMPARE_SUCCESS] = ERRCODE_SUCCESS;
    errorCode_[FI_FACE_NOT_FOUND] = FACE_NOT_FOUND;
    errorCode_[FI_FACE_SCALE_TOO_SMALL] = FACE_SCALE_TOO_SMALL;
    errorCode_[FI_FACE_SCALE_TOO_LARGE] = FACE_SCALE_TOO_LARGE;
    errorCode_[FI_FACE_OFFSET_LEFT] = FACE_OFFSET_LEFT;
    errorCode_[FI_FACE_OFFSET_TOP] = FACE_OFFSET_TOP;
    errorCode_[FI_FACE_OFFSET_RIGHT] = FACE_OFFSET_RIGHT;
    errorCode_[FI_FACE_OFFSET_BOTTOM] = FACE_OFFSET_BOTTOM;
    errorCode_[MG_UNLOCK_ATTR_EYE_OCCLUSION] = FACE_EYE_OCCLUSION;
    errorCode_[MG_UNLOCK_ATTR_EYE_CLOSE] = FACE_EYE_CLOSE;
    errorCode_[MG_UNLOCK_ATTR_MOUTH_OCCLUSION] = FACE_EYE_MOUTH_OCCLUSION;
    errorCode_[MG_UNLOCK_FACE_NOT_COMPLETE] = FACE_NOT_COMPLETE;
    errorCode_[FI_FACE_TOO_DARK] = FACE_UNLOCK_FACE_DARKLIGHT;
    errorCode_[MG_UNLOCK_HIGHLIGHT] = FACE_UNLOCK_FACE_HIGHTLIGHT;
    errorCode_[MG_UNLOCK_HALF_SHADOW] = FACE_UNLOCK_FACE_HALF_SHADOW;
    errorCode_[FI_NOT_GAZE] = FACE_NOT_GAZE;
    errorCode_[FI_FACE_ROTATE_TOP_RIGHT] = FACE_ROTATE_TOP_RIGHT;
    errorCode_[FI_FACE_ROTATE_TOP_LEFT] = FACE_ROTATE_TOP_LEFT;
    errorCode_[FI_FACE_ROTATE_BOTTOM_RIGHT] = FACE_ROTATE_BOTTOM_RIGHT;
    errorCode_[FI_FACE_ROTATE_BOTTOM_LEFT] = FACE_ROTATE_BOTTOM_LEFT;
    errorCode_[FI_FACE_FACE_SHADE] = FACE_WITHOUT_MASK;
    errorCode_[FI_FACE_INFO] = FACE_COVERED_WITH_MASK;
    errorCode_[MG_UNLOCK_FACE_BAD_QUALITY] = FACE_BAD_QUALITY;
    errorCode_[FI_COMPARE_FAIL] = FACE_LIVENESS_FAILURE;
    errorCode_[FI_FACE_ROTATE_LEFT] = FACE_ROTATED_LEFT;
    errorCode_[MG_UNLOCK_FACE_RISE] = FACE_RISE;
    errorCode_[MG_UNLOCK_FACE_ROTATED_RIGHT] = FACE_ROTATED_RIGHT;
    errorCode_[MG_UNLOCK_FACE_DOWN] = FACE_DOWN;
    errorCode_[MG_UNLOCK_FACE_MULTI] = FACE_MULTI;
    errorCode_[MG_UNLOCK_FACE_BLUR] = FACE_BLUR;
    errorCode_[FI_ENROLL_FACE_ANGLE_OK] = FACE_ANGLE_BASE;
    errorCode_[FI_FACE_INFO] = FACE_ENROLL_INFO_BEGIN;
    errorCode_[FI_ENROLL_SUCCESS] = ERRCODE_SUCCESS;
    errorCode_[FI_FACE_DETECTED] = FACE_DETECTED;
    errorCode_[FI_FACE_OUT_OF_BOUNDS] = FACE_OUT_OF_BOUNDS;
    errorCode_[FI_FACE_DARKPIC] = FACE_DARKPIC;
    errorCode_[IFACE_ENROLL_HAS_REGISTERED] = ERRCODE_ENROLL_HAS_REGISTERED;
}

FIRetCode FaceAuthAlgoAdapter::GetEnrollResult(int32_t &authErrorCode, FICode &code, uint64_t reqId)
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoAdapter::GetEnrollResult start");
    int32_t resultCode = 0;
    int32_t params[ALO_GETRESULT_PARAM_LEN];
    if (isCancel_) {
        if (cancelReqId_ != reqId) {
            isCancel_ = false;
            FACEAUTH_LABEL_LOGW(
                "cancelReqId_ and reqId are different. cancelReqId_ is :xxxx%04llu,reqId is :xxxx%04llu", cancelReqId_,
                reqId);
        }
        authErrorCode = ERRCODE_CANCEL;
        code = CODE_CALLBACK_RESULT;
        isCancel_ = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        return FI_RC_OK;
    }
    FIRetCode result = GetResult(resultCode, params);
    if (result != FI_RC_OK) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::GetResult failed");
    }

    if (authErrorCode == FACE_AUTH_GETRESULT_FAIL) {
        authErrorCode = ERRCODE_FAIL;
        code = CODE_CALLBACK_RESULT;
        return result;
    }

    if (authErrorCode == FACE_AUTH_GETRESULT_TIMEOUT) {
        authErrorCode = ERRCODE_TIMEOUT;
        code = CODE_CALLBACK_RESULT;
        return result;
    }

    if (errorCode_.find(resultCode) != errorCode_.end()) {
        authErrorCode = errorCode_[resultCode];
    } else {
        authErrorCode = resultCode;
    }
    CheckResultCode(resultCode, authErrorCode, code, params);
    FACEAUTH_LABEL_LOGI("reqid_ is xxxx%04llu,code_ is %{public}d,errorcode_ is %{public}d ", reqId, code,
        authErrorCode);
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoAdapter::GetEnrollResult end");
    return result;
}

void FaceAuthAlgoAdapter::CheckResultCode(int32_t resultCode, int32_t &authErrorCode, FICode &code,
    int32_t params[ALO_GETRESULT_PARAM_LEN])
{
    switch (resultCode) {
        case IFACE_OVER_MAX_FACES:
            authErrorCode = ERRCODE_OVER_MAX_FACES;
            code = CODE_CALLBACK_RESULT;
            break;
        case IFACE_ENROLL_HAS_REGISTERED:
            authErrorCode = ERRCODE_ENROLL_HAS_REGISTERED;
            code = CODE_CALLBACK_RESULT;
            break;
        case IFACE_CAMERA_FAIL:
            authErrorCode = FACE_OPEN_CAMERA_FAILED;
            code = CODE_CALLBACK_ACQUIRE;
            break;
        case FI_ENROLL_FAIL:
            authErrorCode = ERRCODE_FAIL;
            code = CODE_CALLBACK_RESULT;
            break;
        case FI_ENROLL_SUCCESS:
            if (!isReturnFaceId_) {
                authErrorCode = params[0];
                code = CODE_CALLBACK_FACEID;
                isReturnFaceId_ = true;
            } else {
                code = CODE_CALLBACK_RESULT;
            }
            break;
        case IFACE_TIME_OUT:
            authErrorCode = ERRCODE_TIMEOUT;
            code = CODE_CALLBACK_RESULT;
            break;
        default:
            code = CODE_CALLBACK_ACQUIRE;
            break;
    }
}

FIRetCode FaceAuthAlgoAdapter::GetAuthResult(int32_t &authErrorCode, FICode &code, uint64_t reqId)
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoAdapter::GetAuthResult start");
    int32_t resultCode = 0;
    int32_t params[ALO_GETRESULT_PARAM_LEN] = {0};
    if (!CheckIsCancel(authErrorCode, code, reqId)) {
        return FI_RC_OK;
    }
    FIRetCode result = GetResult(resultCode, params);
    if (result != FI_RC_OK) {
        FACEAUTH_LABEL_LOGE("FaceAuthManager::GetCodeResult failed");
    }
    FACEAUTH_LABEL_LOGI("params[0] is %{public}d", params[0]);
    if (resultCode == FI_COMPARE_FAIL) {
        authErrorCode = SwitchAuthErrorCode(params[0]);
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (errorCode_.find(resultCode) != errorCode_.end()) {
        authErrorCode = errorCode_[resultCode];
    } else {
        authErrorCode = resultCode;
    }
    if (authErrorCode == FACE_AUTH_GETRESULT_FAIL) {
        authErrorCode = ERRCODE_FAIL;
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (authErrorCode == FACE_AUTH_GETRESULT_TIMEOUT) {
        authErrorCode = ERRCODE_TIMEOUT;
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (authErrorCode == IFACE_TIME_OUT) {
        authErrorCode = ERRCODE_TIMEOUT;
        code = CODE_CALLBACK_RESULT;
        return result;
    }
    if (authErrorCode == IFACE_CAMERA_FAIL) {
        authErrorCode = ERRCODE_CAMERA_FAIL;
    }
    if (authErrorCode == ERRCODE_SUCCESS || authErrorCode == ERRCODE_CAMERA_FAIL) {
        code = CODE_CALLBACK_RESULT;
    } else {
        code = CODE_CALLBACK_ACQUIRE;
    }
    FACEAUTH_LABEL_LOGI("reqid_ is xxxx%04llu,code_ is %{public}d,errorcode_ is %{public}d ", reqId, code,
        authErrorCode);
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoAdapter::GetAuthResult end");
    return result;
}

int32_t FaceAuthAlgoAdapter::CheckIsCancel(int32_t &authErrorCode, FICode &code, uint64_t reqId)
{
    if (isCancel_) {
        if (cancelReqId_ != reqId) {
            isCancel_ = false;
            FACEAUTH_LABEL_LOGW(
                "cancelReqId_ and reqId are different. cancelReqId_ is :xxxx%04llu,reqId is :xxxx%04llu", cancelReqId_,
                reqId);
        }
        authErrorCode = ERRCODE_CANCEL;
        code = CODE_CALLBACK_RESULT;
        isCancel_ = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        return FI_RC_OK;
    }
    return FI_RC_ERROR;
}

int32_t FaceAuthAlgoAdapter::SwitchAuthErrorCode(int32_t param)
{
    int32_t authErrorCode = 0;
    switch (param) {
        case FI_FAIL_REASON_FACE_NOT_MATCH:
            authErrorCode = ERRCODE_COMPARE_FAIL;
            break;
        case FI_FAIL_REASON_FACE_MOVED:
            authErrorCode = FACE_MOVED;
            break;
        case FI_FAIL_REASON_NOT_GAZE:
            authErrorCode = ERRCODE_NOT_GAZE;
            break;
        case FI_FAIL_REASON_TIME_OUT:
            authErrorCode = ERRCODE_TIMEOUT;
            break;
        case FI_FAIL_REASON_FACE_OUT_OF_BOUNDS:
        case FI_FAIL_REASON_FACE_NOT_FOUND:
        case FI_FAIL_REASON_LIVENESS_FAILURE:
            authErrorCode = ERRCODE_FAIL;
            break;
        default:
            authErrorCode = ERRCODE_NO_FACE_DATA;
            break;
    }
    return authErrorCode;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS