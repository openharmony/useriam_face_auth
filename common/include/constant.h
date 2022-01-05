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

#ifndef CONSTANT_H
#define CONSTANT_H

#include <algorithm>
#include <map>
#include <string>

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class Constant {
public:
    enum ServiceId {
        SUBSYS_AAFWK_SYS_ABILITY_ID_BEGIN = 100,
        ABILITY_TOOLS_SERVICE_ID = 116,
        ABILITY_TEST_SERVICE_ID = 179,
        ABILITY_MGR_SERVICE_ID = 180,
        ABILITY_MST_SERVICE_ID = 181,
        SUBSYS_AAFWK_SYS_ABILITY_ID_END = 199,
        SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN = 200,
        SUBSYS_AI_SYS_ABILITY_ID_BEGIN = 300,
        SUBSYS_APPEXECFWK_SYS_ABILITY_ID_BEGIN = 400,
        BUNDLE_MGR_SERVICE_SYS_ABILITY_ID = 401,
        SUBSYS_APPLICATIONS_SYS_ABILITY_ID_BEGIN = 500,
        APP_MGR_SERVICE_ID = 501,
        INSTALLD_SERVICE_ID = 511,
        SUBSYS_ARVR_SYS_ABILITY_ID_BEGIN = 600,
        SUBSYS_ARVRHARDWARE_SYS_ABILITY_ID_BEGIN = 700,
        SUBSYS_BARRIERFREE_SYS_ABILITY_ID_BEGIN = 800,
        SUBSYS_FACEAUTHS_SYS_ABILITY_ID_BEGIN = 900,
        SUBSYS_FACEAUTHS_SYS_ABILITY_FACERECOGNIZE = 901,
        SUBSYS_FACEAUTHS_SYS_ABILITY_FINGERPRINT = 902,
        SUBSYS_FACEAUTHS_SYS_ABILITY_VOICEID = 903,
        ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID = 3203,
        SUBSYS_SECURITY_PERMISSION_SYS_SERVICE_ID = 3501,
    };

    /**
     * Status code, indicates general success.
     */
    const static int32_t SUCCESS = 0;

    /**
     * Status code, indicates general failure.
     */
    const static int32_t FAILURE = -1;

    /**
     * Status code, indicates JS general failure.
     */
    const static int32_t JS_FAILURE = 1;

    /**
     * Callback code.
     */
    const static int32_t CODE_RESULT = 1;
    const static int32_t CODE_CANCEL = 2;
    const static int32_t CODE_ACQUIRE = 3;
    const static int32_t CODE_BUSY = 4;

    /**
     * Callback type.
     */
    const static int32_t TYPE_ENROLL = 0;
    const static int32_t TYPE_AUTH = 1;
    const static int32_t TYPE_REMOVE = 2;

    /**
     * Authentication type.
     */
    static const std::string FACE_ONLY;
    static const std::string ALL;
    static const std::string APP_PERMISSION_TYPE;

    /**
     * Authentication level.
     */
    static const std::string LEVEL_1;
    static const std::string LEVEL_2;
    static const std::string LEVEL_3;
    static const std::string LEVEL_4;

    /**
     * GetLocalDeviceId
     */
    static std::string GetLocalDeviceId();

    /**
     * Authentication permission name
     */
    const static std::string ACCESS_PERMISSION_NAME;

    /**
     * Enroll permission name
     */
    const static std::string ENROLL_PERMISSION_NAME;

    /**
     * Code and errorCode translate into tipCode.
     */
    const static std::map<std::string, int32_t> TIP_CODE_MAP;

    /**
     * Code and errorCode translate into tipInfo.
     */
    const static std::map<std::string, std::string> TIP_INFO_MAP;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS
#endif  // PERMISSION_BASE_SERVICE_H