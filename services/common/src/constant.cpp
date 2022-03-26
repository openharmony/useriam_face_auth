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

#include "constant.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const std::map<std::string, int32_t> Constant::TIP_CODE_MAP = {
    std::map<std::string, int32_t>::value_type("1+-1", -1),  // code 1, errorCode -1, means tipCode NO_SUPPORT(-1)
    std::map<std::string, int32_t>::value_type("1+0", 0),    // code 1, errorCode 0, means tipCode SUCCESS(0)
    std::map<std::string, int32_t>::value_type("1+3", 1),    // code 1, errorCode 3, means tipCode COMPARE_FAILURE(1)
    std::map<std::string, int32_t>::value_type("1+2", 2),    // code 1, errorCode 2, means tipCode CANCELED(2)
    std::map<std::string, int32_t>::value_type("1+4", 3),    // code 1, errorCode 4, means tipCode TIMEOUT(3)
    std::map<std::string, int32_t>::value_type("1+12", 4),   // code 1, errorCode 12, means tipCode CAMERA_FAIL(4)
    std::map<std::string, int32_t>::value_type("1+13", 5),   // code 1, errorCode 13, means tipCode BUSY(5)
    std::map<std::string, int32_t>::value_type("1+9", 6),    // code 1, errorCode 9, means tipCode INVALID_PARAMETERS(6)
    std::map<std::string, int32_t>::value_type("1+8", 7),    // code 1, errorCode 8, means tipCode LOCKED(7)
    std::map<std::string, int32_t>::value_type("1+10", 8),   // code 1, errorCode 10, means tipCode NOT_ENROLLED(8)
    std::map<std::string, int32_t>::value_type("1+5", 100),  // code 1, errorCode 5, means tipCode GENERAL_ERROR(100)
    std::map<std::string, int32_t>::value_type("1+16", 100),  // code 1, errorCode 16, means tipCode GENERAL_ERROR(100)
    std::map<std::string, int32_t>::value_type("1+7", 100),   // code 1, errorCode 7, means tipCode GENERAL_ERROR(100)
    std::map<std::string, int32_t>::value_type("1+11", 100),  // code 1, errorCode 11, means tipCode GENERAL_ERROR(100)
    std::map<std::string, int32_t>::value_type("2+0", 0),     // code 2, errorCode 0, means tipCode SUCCESS(0)
    std::map<std::string, int32_t>::value_type(
        "3+31", 1),  // code 3, errorCode 31, means tipCode FACE_AUTH_TIP_TOO_BRIGHT(1)
    std::map<std::string, int32_t>::value_type(
        "3+30", 2),  // code 3, errorCode 30, means tipCode FACE_AUTH_TIP_TOO_DARK(2)
    std::map<std::string, int32_t>::value_type(
        "3+7", 3),  // code 3, errorCode 7, means tipCode FACE_AUTH_TIP_TOO_CLOSE(3)
    std::map<std::string, int32_t>::value_type(
        "3+6", 4),  // code 3, errorCode 6, means tipCode FACE_AUTH_TIP_TOO_FAR(4)
    std::map<std::string, int32_t>::value_type(
        "3+9", 5),  // code 3, errorCode 9, means tipCode FACE_AUTH_TIP_TOO_HIGH(5)
    std::map<std::string, int32_t>::value_type(
        "3+11", 6),  // code 3, errorCode 11, means tipCode FACE_AUTH_TIP_TOO_LOW(6)
    std::map<std::string, int32_t>::value_type(
        "3+10", 7),  // code 3, errorCode 10, means tipCode FACE_AUTH_TIP_TOO_RIGHT(7)
    std::map<std::string, int32_t>::value_type(
        "3+8", 8),  // code 3, errorCode 8, means tipCode FACE_AUTH_TIP_TOO_LEFT(8)
    std::map<std::string, int32_t>::value_type(
        "3+22", 9),  // code 3, errorCode 22, means tipCode FACE_AUTH_TIP_TOO_MUCH_MOTION(9)
    std::map<std::string, int32_t>::value_type(
        "3+36", 10),  // code 3, errorCode 36, means tipCode FACE_AUTH_TIP_POOR_GAZE(10)
    std::map<std::string, int32_t>::value_type(
        "3+5", 11),  // code 3, errorCode 5, means tipCode FACE_AUTH_TIP_NOT_DETECTED(11)
    std::map<std::string, int32_t>::value_type("4+0", 0),  // code 4, errorCode 0, means tipCode SUCCESS(0)
};

const std::map<std::string, std::string> Constant::TIP_INFO_MAP = {
    std::map<std::string, std::string>::value_type("1+-1", "not support"),
    std::map<std::string, std::string>::value_type("1+0", "success"),
    std::map<std::string, std::string>::value_type("1+3", "compare fail"),
    std::map<std::string, std::string>::value_type("1+2", "cancelled"),
    std::map<std::string, std::string>::value_type("1+4", "timeout"),
    std::map<std::string, std::string>::value_type("1+12", "open camera fail"),
    std::map<std::string, std::string>::value_type("1+13", "busy"),
    std::map<std::string, std::string>::value_type("1+9", "invalid parameters"),
    std::map<std::string, std::string>::value_type("1+8", "locked"),
    std::map<std::string, std::string>::value_type("1+10", "not enrolled"),
    std::map<std::string, std::string>::value_type("1+5", "general error"),
    std::map<std::string, std::string>::value_type("1+16", "general error"),
    std::map<std::string, std::string>::value_type("1+7", "general error"),
    std::map<std::string, std::string>::value_type("1+11", "general error"),
    std::map<std::string, std::string>::value_type("2+0", "success"),
    std::map<std::string, std::string>::value_type("3+31", "too bright"),
    std::map<std::string, std::string>::value_type("3+30", "too dark"),
    std::map<std::string, std::string>::value_type("3+7", "too close"),
    std::map<std::string, std::string>::value_type("3+6", "too far"),
    std::map<std::string, std::string>::value_type("3+9", "too high"),
    std::map<std::string, std::string>::value_type("3+11", "too low"),
    std::map<std::string, std::string>::value_type("3+10", "too right"),
    std::map<std::string, std::string>::value_type("3+8", "too left"),
    std::map<std::string, std::string>::value_type("3+22", "too much motion"),
    std::map<std::string, std::string>::value_type("3+36", "poor gaze"),
    std::map<std::string, std::string>::value_type("3+5", "face not detected"),
    std::map<std::string, std::string>::value_type("4+0", "success"),
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS
