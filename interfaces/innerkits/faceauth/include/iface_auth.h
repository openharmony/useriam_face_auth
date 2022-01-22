/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef FACEAUTH_INNERKITS_INCLUDE_IFACE_AUTH_H
#define FACEAUTH_INNERKITS_INCLUDE_IFACE_AUTH_H
#include <list>
#include <vector>
#include "face_auth_defines.h"
#include "on_faceauth.h"
#include "iremote_broker.h"
#include "iremote_object.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class IFaceAuth : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.faceauth.IFaceAuth");

    /**
     * @brief Initialize the face algorithm and synchronize the interface.
     *
     * @return 0 success, -1 failure.
     */
    virtual int32_t Init() = 0;

    /**
     * @brief Release the face algorithm, synchronize the interface.
     *
     * @return 0 success, -1 failure.
     */
    virtual int32_t Release() = 0;

    /**
     * @brief Enroll,asynchronous interface, system permission control is required.
     *
     * @param param.reqId Request Id
     * @param param.flags Flags.
     * @param param.token Enter the authentication string of the face.
     * @param param.preview Receiving memory of face preview image stream.
     * @param param.faceId Specified face ID.
     * @param callback The callback function provided for caller to handle response data.
     * @return 0 success, -1 failure.
     */
    virtual int32_t Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback) = 0;

    /**
     * @brief Authentication, asynchronous interface.
     *
     * @param param.reqId Request ID,need to pass in the same ID when canceling.
     * @param param.flags Flags.
     * @param param.preview Receiving memory of face preview image stream.
     * @param param.challenge Authentication random challenge for issuing authToken.
     * @param param.faceId Specified face ID.
     * @param callback The callback function provided for caller to handle response data.
     * @return 0 success, -1 failure.
     */
    virtual int32_t Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback) = 0;

    /**
     * @brief Canceling an ongoing entry request requires system permission control for asynchronous interfaces.
     *
     * @param reqId Request ID.Consistent with enroll ID.
     * @return 0 success, -1 failure.
     */
    virtual int32_t CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback) = 0;

    /**
     * @brief Cancel the ongoing authentication request, asynchronous interface.
     *
     * @param reqId Request ID.Consistent with authentication ID.
     * @param callback The callback function provided for caller to handle response data.
     * @return 0 success, -1 failure.
     */
    virtual int32_t CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback) = 0;

    /**
     * @brief Ready to enroll face.
     *
     * @return 0 success, -1 failure.
     */
    virtual int64_t PreEnroll() = 0;

    /**
     * @brief Post enroll and remove the challenge generated at the bottom of the pre enroll.
     *
     * @return 0 success, -1 failure.
     */
    virtual int32_t PostEnroll() = 0;

    /**
     * @brief Delete enrolled face by the current user, asynchronous interface.
     *
     * @param reqId Request ID.
     * @param faceId Specified face ID.
     * @return 0 success, -1 failure.
     */
    virtual int32_t Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback) = 0;

    /**
     * @brief Obtain the entered face template ID list and synchronize the interface.
     *
     * @return 0 success, -1 failure.
     */
    virtual std::list<int32_t> GetEnrolledFaceIDs(const int32_t userId = 0) = 0;

    /**
     * @brief Unfreezing,resetting the face error count and synchronizing the interface require system permission
     * control.
     *
     * @return 0 success, -1 failure.
     */
    virtual void ResetTimeout() = 0;

    /**
     * @brief The anti brute force cracking.Get Remaining Number.
     *
     * @return 0 success, -1 failure.
     */
    virtual int32_t GetRemainingNum() = 0;

    /**
     * @brief The anti brute force cracking.Get Remaining Time.
     *
     * @return 0 success, -1 failure.
     */
    virtual int64_t GetRemainingTime() = 0;

    /**
     * @brief Obtain the number of face angles to be entered.
     *
     * @return 0 success, -1 failure.
     */
    virtual int32_t GetAngleDim() = 0;

    enum {
        FACE_AUTH_SERVICE_TEST = 0,
        FACE_AUTH_AUTHENTICATE = 1,
        FACE_AUTH_CANCEL_AUTH = 2,
        FACE_AUTH_INIT = 3,
        FACE_AUTH_RELEASE = 4,
        FACE_AUTH_RESET_TIMEOUT = 5,
        FACE_AUTH_GET_REMAINING_NUM = 6,
        FACE_AUTH_GET_REMAINING_TIME = 7,
        FACE_AUTH_PREENROLL = 8,
        FACE_AUTH_POSTENROLL = 9,
        FACE_AUTH_REMOVE = 10,
        FACE_AUTH_GET_ENROLLED_FACEIDS = 11,
        FACE_AUTH_GET_ANGLEDIM = 12,
        FACE_AUTH_ENROLL = 13,
        FACE_AUTH_CANCEL_ENROLLMENT = 14,
    };
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_INNERKITS_INCLUDE_IFACE_AUTH_H