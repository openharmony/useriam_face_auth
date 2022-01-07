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

#ifndef FACE_AUTH_DEFINES_H
#define FACE_AUTH_DEFINES_H

#include <vector>
#include "on_faceauth.h"
#include "ibuffer_producer.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
typedef enum {
    FACE_INVALID_OPERATE_TYPE = 0,
    FACE_OPERATE_TYPE_ENROLL,
    FACE_OPERATE_TYPE_LOCAL_AUTH,
    FACE_OPERATE_TYPE_CO_AUTH,
    FACE_OPERATE_TYPE_DEL,
    FACE_OPERATE_TYPE_MAX,
} FaceOperateType;

typedef enum {
    SUPPORTED = 0,
    AUTH_TYPE_NOT_SUPPORT,
    SECURE_LEVEL_NOT_SUPPORT,
    DISTRIBUTED_AUTH_NOT_SUPPORT,
    NOT_ENROLLED,
} AvailabilityCode;
typedef struct faceReqType {
    uint64_t reqId = 0;
    FaceOperateType operateType = FACE_INVALID_OPERATE_TYPE;
    bool operator<(const faceReqType &s) const
    {
        if (this->reqId < s.reqId) {
            return true;
        }
        return false;
    }
} FaceReqType;
typedef struct faceInfo {
    uint32_t eventId = 0;
    int32_t uId = 0;
    bool isCanceled = false;
} FaceInfo;
// constant
namespace {
static const int32_t FA_RET_OK = 0;
static const int32_t FA_RET_ERROR = -1;
static const int32_t IMAGE_STREAM_EXTRA_SIZE = 10;
static const int32_t RESULT_MAX_SIZE = 10;
static const int32_t ENROLL_FACE_REC_SIZE = 4;
static const int32_t UNUSED_PARAM_SIZE = 4;
static const int32_t AUTH_FAIL_MAX_TIMES = 5;
static const int32_t OPEN_CAMERA_TIME_OUT = 11000000;
static const int32_t INIT_DYNAMIC_TIME_OUT = 5000000;
static const int32_t GET_RESULT_TIME_OUT = 30000000;
static const int32_t AUTH_FAIL_WAIT_TIME = 30;
static const int32_t ALO_GETRESULT_PARAM_LEN = 10;
static const int32_t ALO_GETRESULT_PARAM_CAR_FAIL_REASON = 1;
static const uint32_t ENROLL_ANGLE_MAX_NUM = 9;
static const int32_t CO_AUTH_MAX_NUM = 3;
static const int32_t PARAM_NUM = 10;
constexpr static double MAX_ELAPSED = 2;
static const int32_t USER_ID_DEFAULT = 0;
static const int32_t TOKEN_NUM = 69;
static const int32_t PREVIEW_DEFAULT_WIDTH = 640;
static const int32_t PREVIEW_DEFAULT_HEIGHT = 480;
static const int32_t FACE_AUTH_THREAD_NUM = 3;
static const int64_t SEC_TO_NANOSEC = 1000000000;
}  // namespace
struct AuthParam {
    uint64_t reqId = 0;
    int32_t flags = 0;
    sptr<IBufferProducer> producer = nullptr;
    int64_t challenge = 0;
    int32_t faceId = 0;
};
struct EnrollParam {
    uint64_t reqId = 0;
    sptr<IBufferProducer> producer = nullptr;
    int64_t challenge = 0;
    int32_t faceId = 0;
    std::vector<uint8_t> token;
};
struct RemoveParam {
    uint64_t reqId = 0;
    int32_t faceId = 0;
};
struct CallsAuthInfo {
    AuthParam param;
    sptr<OnFaceAuth> callback;
    CallsAuthInfo(const AuthParam _param, const sptr<OnFaceAuth> _callback)
    {
        param = _param;
        callback = _callback;
    }
};
struct CallsEnrollInfo {
    EnrollParam param;
    sptr<OnFaceAuth> callback;
    CallsEnrollInfo(const EnrollParam _param, const sptr<OnFaceAuth> _callback)
    {
        param = _param;
        callback = _callback;
    }
};
struct CallsRemoveInfo {
    RemoveParam param;
    sptr<OnFaceAuth> callback;
    CallsRemoveInfo(const RemoveParam _param, const sptr<OnFaceAuth> _callback)
    {
        param = _param;
        callback = _callback;
    }
};
typedef struct callBackParam {
    uint64_t reqId = 0;
    int32_t code = 0;
    int32_t errorCode = 0;
    int32_t flags = 0;
    std::vector<uint8_t> authtoken;
} CallBackParam;
typedef enum FIRetCode {
    FI_RC_OK = 0,
    FI_RC_ERROR,
    FI_RC_INVALID_ARGUMENT,
    FI_RC_INVALID_HANDLE,
    FI_RC_NULLPOINTER,
    FI_RC_OUTORMEMORY,
} FIRetCode;
typedef struct PrepareParam {
    bool isNeedGaze;       // Need to look at the camera
    bool isMaskMode;       // Mask mode
    int32_t authProtocol;  // Authentication protocol
    int32_t faceId;        // Face ID
    int32_t enrollFaceRec[ENROLL_FACE_REC_SIZE];
    int32_t sensorType;                      // Camera type
    int32_t skipEyeOpenCheck;                // Skip eye opening check
    int32_t unusedParam[UNUSED_PARAM_SIZE];  // Reserved parameters
} PrepareParam;
typedef enum HWExeType {
    HW_EXEC_TYPE_ENROOL = 0,
    HW_EXEC_TYPE_UNLOCK,
    HW_EXEC_TYPE_PAY,
    HW_EXEC_TYPE_LEARN,
} HWExeType;
typedef struct CameraImageStream {
    int32_t secureType; /* 0: normal 1: secure */
    int32_t streamType;
    int64_t timestamp;
    uint32_t width;
    uint32_t height;
    int32_t format;
    union data {
        int32_t sharedFd;
        void *image;
        int32_t bufferIndex;
    } data;
    int32_t offset;
    uint32_t size;
    int32_t irStatus;
    int32_t envLux;
    uint32_t stride;
    int32_t dotTemperature;
    int32_t irSensorTemperature;
    uint32_t distance;
    int32_t isExposureValid; /* AE status: 0 invalid, 1 valid */
    int32_t bufferType;      /* 0 dynamic memory, 1 static memory, 2 remote image */
    int32_t extra[IMAGE_STREAM_EXTRA_SIZE];
} CameraImageStream;
typedef enum FIResultCode {
    FI_NONE = 0,
    FI_FACE_INFO,
    FI_FACE_NOT_FOUND,
    FI_FACE_SCALE_TOO_SMALL,
    FI_FACE_SCALE_TOO_LARGE,
    FI_FACE_OFFSET_LEFT,
    FI_FACE_OFFSET_TOP,
    FI_FACE_OFFSET_RIGHT,
    FI_FACE_OFFSET_BOTTOM,
    FI_NOT_GAZE,
    FI_ENROLL_FACE_ANGLE_OK,
    FI_ENROLL_NOT_THE_SAME_FACE,
    FI_ENROLL_TOO_MANY_FACE,
    FI_FACE_TOO_DARK,
    FI_FACE_FACE_SHADE,
    FI_ENROLL_SUCCESS,
    FI_ENROLL_FAIL,
    FI_COMPARE_SUCCESS,
    FI_COMPARE_FAIL,
    FI_MSG_TO_DEPTH,
    FI_FRAME_COMPARE_FAIL,
    FI_FRAME_LIVENESS_FAIL,
    FI_FACE_OUT_OF_BOUNDS,
    FI_FACE_ROTATE_LEFT,
    FI_FACE_ROTATE_TOP,
    FI_FACE_ROTATE_RIGHT,
    FI_FACE_ROTATE_BOTTOM,
    FI_FACE_ROTATE_TOP_RIGHT,
    FI_FACE_ROTATE_TOP_LEFT,
    FI_FACE_ROTATE_BOTTOM_RIGHT,
    FI_FACE_ROTATE_BOTTOM_LEFT,
    FI_FACE_DARKPIC,
    FI_FACE_DETECTED = 38,
} FIResultCode;
typedef enum RESULT_ERRORCODE {
    FACE_NONE = -1,
    ERRCODE_SUCCESS = 0,
    ERRCODE_FAIL,
    ERRCODE_CANCEL,
    ERRCODE_COMPARE_FAIL,
    ERRCODE_TIMEOUT,
    ERRCODE_ALGORITHM_NOT_INIT,
    ERRCODE_HAL_INVALIDE,
    ERRCODE_OVER_MAX_FACES,
    ERRCODE_IN_LOCKOUT_MODE,
    ERRCODE_INVALID_PARAMETERS,
    ERRCODE_NO_FACE_DATA,
    ERRCODE_NOT_GAZE,
    ERRCODE_CAMERA_FAIL,
    ERRCODE_BUSY,
    ERRCODE_ENROLL_HAS_REGISTERED = 16,
    ERRCODE_COMPARE_OK_NO_TRUSTED_THINGS,
} CALLBACK_RESULT_ERRORCODE;
typedef enum AUTH_CODE_CALLBACK_ACQUIRE {
    FACE_BAD_QUALITY = 4,
    FACE_NOT_FOUND,
    FACE_SCALE_TOO_SMALL,
    FACE_SCALE_TOO_LARGE,
    FACE_OFFSET_LEFT,
    FACE_OFFSET_TOP,
    FACE_OFFSET_RIGHT,
    FACE_OFFSET_BOTTOM,
    FACE_LIVENESS_WARNING = 13,
    FACE_LIVENESS_FAILURE,
    FACE_ROTATED_LEFT,
    FACE_RISE,
    FACE_ROTATED_RIGHT,
    FACE_DOWN,
    FACE_EYE_OCCLUSION = 21,
    FACE_EYE_CLOSE,
    FACE_EYE_MOUTH_OCCLUSION,
    FACE_MULTI = 27,
    FACE_BLUR,
    FACE_NOT_COMPLETE = 29,
    FACE_UNLOCK_FACE_DARKLIGHT,
    FACE_UNLOCK_FACE_HIGHTLIGHT,
    FACE_UNLOCK_FACE_HALF_SHADOW,
    FACE_MOVED,
    FACE_OUT_OF_BOUNDS,
    FACE_NOT_GAZE = 36,
    FACE_HAS_REGISTERED,
    FACE_DETECTED,
    FACE_DARKPIC,
    FACE_ROTATE_TOP_RIGHT,
    FACE_ROTATE_TOP_LEFT,
    FACE_ROTATE_BOTTOM_RIGHT,
    FACE_ROTATE_BOTTOM_LEFT,
    FACE_WITHOUT_MASK,
    FACE_COVERED_WITH_MASK,
    FACE_OPEN_CAMERA_FAILED,
    FACE_ANGLE_BASE = 1000,
    FACE_ENROLL_INFO_BEGIN = 2000,
} AUTH_CODE_CALLBACK_ACQUIRE;
typedef enum FIResultType {
    TYPE_CALLBACK_ENROLL,
    TYPE_CALLBACK_AUTH,
    TYPE_CALLBACK_REMOVE,
} FIResultType;
typedef enum FICode {
    CODE_CALLBACK_START = 0,
    CODE_CALLBACK_RESULT,
    CODE_CALLBACK_CANCEL,
    CODE_CALLBACK_ACQUIRE,
    CODE_CALLBACK_BUSY,
    CODE_CALLBACK_FACEID = 6,
    CODE_CALLBACK_MAX,
} FICode;

typedef enum MGULKStatus {
    MG_UNLOCK_OK = 0x0000,       // <The interface status is normal
    MG_UNLOCK_INVALID_ARGUMENT,  // <Wrong parameter passed in
    MG_UNLOCK_INVALID_HANDLE,    // <The handle object passed in is illegal
    MG_UNLOCK_FAILURE,           // <Other errors, such as improper calling process or expiration
    // compare and save_feature, The following are returned
    MG_UNLOCK_FACE_BAD_QUALITY = 0x1000,  // <Poor picture quality
    MG_UNLOCK_FACE_NOT_FOUND,             // <No found face
    MG_UNLOCK_FACE_SCALE_TOO_SMALL,       // <The face is too small
    MG_UNLOCK_FACE_SCALE_TOO_LARGE,       // <Face too big
    MG_UNLOCK_FACE_OFFSET_LEFT,           // <Face left
    MG_UNLOCK_FACE_OFFSET_TOP,            // <Face up
    MG_UNLOCK_FACE_OFFSET_RIGHT,          // <Face right
    MG_UNLOCK_FACE_OFFSET_BOTTOM,         // <Face down
    MG_UNLOCK_ATTR_BLUR,                  // <Face blur
    MG_UNLOCK_ATTR_EYE_CLOSE,             // <Eyes close
    MG_UNLOCK_FACE_NOT_COMPLETE,          // <Incomplete face
    MG_UNLOCK_LIVENESS_FAILURE,  // <When a possible live attack is detected or the comparison fails, the external
                                 // should
                                 // continue to call this interface to pass in the picture data, and the SDK will judge
                                 // whether it is really a live attack according to the continuous frames
    MG_UNLOCK_KEEP,       // <It is not possible to judge whether it is successful, so it is necessary to continue the
                          // detection
    MG_UNLOCK_DARKLIGHT,  // <Too dark
    // Only save The value returned by the feature
    MG_UNLOCK_ATTR_EYE_OCCLUSION = 0x2000,  // <Eye occlusion
    MG_UNLOCK_ATTR_MOUTH_OCCLUSION,         // Mouth occlusion (compare will return from 2.0.39.1061)
    MG_UNLOCK_FACE_ROTATED_LEFT,            // <Turn left
    MG_UNLOCK_FACE_RISE,                    // <rise
    MG_UNLOCK_FACE_ROTATED_RIGHT,           // <Turn right
    MG_UNLOCK_FACE_DOWN,                    // <Bow your head
    MG_UNLOCK_FACE_MULTI,                   // <Multiple faces
    MG_UNLOCK_FACE_BLUR,                    // <Face blur
    MG_UNLOCK_HIGHLIGHT,                    // <Too bright
    MG_UNLOCK_HALF_SHADOW,                  // <Half shadow
    // Only compare The value returned by the feature
    MG_UNLOCK_COMPARE_FAILURE = 0x3000,  // <Face comparison failed (not the same person)
    // The algorithm has been upgraded and restore needs to be called_ Feature re extracts the bottom library feature
    MG_UNLOCK_NEED_RESTORE_FEATURE,
    MG_UNLOCK_CONTINUE,  // If there are not enough 5 base libraries, you have to continue to enter the base library
    MG_UNLOCK_MAX,
} MGULKStatus;
typedef enum {
    AR_SUCCESS,
    AR_FAIL,
    AR_EMPTY,
    AR_NOT_FOUND,
    AR_ADD_AGAIN,
} AlgoResult;
typedef struct {
    int32_t resultCode;
    int32_t param[RESULT_MAX_SIZE];
} ResultInfo;
typedef enum FIFailReason {
    FI_FAIL_REASON_FACE_NOT_MATCH = 0,
    FI_FAIL_REASON_FACE_MOVED = 1,
    FI_FAIL_REASON_FACE_OUT_OF_BOUNDS = 2,
    FI_FAIL_REASON_FACE_NOT_FOUND = 3,
    FI_FAIL_REASON_NOT_GAZE = 4,
    FI_FAIL_REASON_LIVENESS_FAILURE = 5,
    FI_FAIL_REASON_TIME_OUT = 6,
    FI_FAIL_REASON_NEED_UPGRADE = 7,
} FIFailReason;
typedef enum FaceErrorCode {
    FACE_SUCCESS = 0,        /* Used for init, enroll, authenticate, remove success */
    FACE_ERROR_FAIL,         /* Used for init, enroll, authenticate, remove fail */
    FACE_ERROR_CANCEL,       /* Operation was canceled */
    FACE_ERROR_COMPARE_FAIL, /* Compare fail, only for authenticate **/
    FACE_ERROR_TIMEOUT,      /* timeout  */
    FACE_ERROR_NOT_INIT,
    FACE_SUCCESS_NEED_UPGRADE = 14,
    FACE_SUCCESS_NO_NEED_UPGRADE = 15,
    FACE_ENROLL_HAS_REGISTERED = 16, /* For car only */
} FaceErrorCode;
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS
#endif