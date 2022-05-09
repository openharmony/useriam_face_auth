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

#ifndef COMMON_H
#define COMMON_H

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
#include <cstdint>

// constant
namespace {
static const int32_t FA_RET_OK = 0;
static const int32_t FA_RET_ERROR = -1;
} // namespace

enum FaceAuthRet {
    FACEAUTH_SUCCESS = 0,
    FACEAUTH_ERROR = 1,
};

enum ResultCode {
    /**
     * Indicates that authentication is success or ability is supported.
     */
    SUCCESS = 0,
    /**
     * Indicates the authenticator fails to identify user.
     */
    FAIL = 1,
    /**
     * Indicates other errors.
     */
    GENERAL_ERROR = 2,
    /**
     * Indicates that authentication has been canceled.
     */
    CANCELED = 3,
    /**
     * Indicates that authentication has timed out.
     */
    TIMEOUT = 4,
    /**
     * Indicates that this authentication type is not supported.
     */
    TYPE_NOT_SUPPORT = 5,
    /**
     * Indicates that the authentication trust level is not supported.
     */
    TRUST_LEVEL_NOT_SUPPORT = 6,
    /**
     * Indicates that the authentication task is busy. Wait for a few seconds and try again.
     */
    BUSY = 7,
    /**
     * Indicates incorrect parameters.
     */
    INVALID_PARAMETERS = 8,
    /**
     * Indicates that the authenticator is locked.
     */
    LOCKED = 9,
    /**
     * Indicates that the user has not enrolled the authenticator.
     */
    NOT_ENROLLED = 10,
    /**
     * Indicates that the operation is not supported.
     */
    OPERATION_NOT_SUPPORT = 11,
    /**
     * Vendor may add result code after this.
     */
    VENDOR_RESULT_CODE_BEGIN = 10000
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // COMMON_H
