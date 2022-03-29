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

#ifndef FACE_AUTH_LOG_WRAPPER_H
#define FACE_AUTH_LOG_WRAPPER_H

#define CONFIG_HILOG
#ifdef CONFIG_HILOG

#include <inttypes.h>
#include <string>
#include <securec.h>
#include "hilog/log.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
#define FILENAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define FORMATTED(fmt, ...) "[%{public}s] %{public}s# " fmt, FILENAME, __FUNCTION__, ##__VA_ARGS__

#ifdef FACEAUTH_HILOGF
#undef FACEAUTH_HILOGF
#endif

#ifdef FACEAUTH_HILOGE
#undef FACEAUTH_HILOGE
#endif

#ifdef FACEAUTH_HILOGW
#undef FACEAUTH_HILOGW
#endif

#ifdef FACEAUTH_HILOGI
#undef FACEAUTH_HILOGI
#endif

#ifdef FACEAUTH_HILOGD
#undef FACEAUTH_HILOGD
#endif


enum FaceAuthSubModule {
    MODULE_SERVICE = 0,
    MODULE_FRAMEWORK = 1,
    MODULE_MAX
};


static constexpr unsigned int BASE_FACE_AUTH_DOMAIN_ID = 0xD002422;

enum FaceAuthDomainId {
    FACE_AUTH_SERVICE_DOMAIN = BASE_FACE_AUTH_DOMAIN_ID + MODULE_SERVICE,
    FACE_AUTH_FRAMEWORK_DOMAIN = BASE_FACE_AUTH_DOMAIN_ID + MODULE_FRAMEWORK,
    FACE_AUTH_DOMAIN_ID_MAX
};

static constexpr OHOS::HiviewDFX::HiLogLabel FACE_AUTH_LABEL[MODULE_MAX] = {
    {LOG_CORE, FACE_AUTH_SERVICE_DOMAIN, "FaceAuthService"},
    {LOG_CORE, FACE_AUTH_FRAMEWORK_DOMAIN, "FaceAuthFramework"},
};

#define FACEAUTH_HILOGF(module, ...) (void)OHOS::HiviewDFX::HiLog::Fatal(FACE_AUTH_LABEL[module], \
    FORMATTED(__VA_ARGS__))
#define FACEAUTH_HILOGE(module, ...) (void)OHOS::HiviewDFX::HiLog::Error(FACE_AUTH_LABEL[module], \
    FORMATTED(__VA_ARGS__))
#define FACEAUTH_HILOGW(module, ...) (void)OHOS::HiviewDFX::HiLog::Warn(FACE_AUTH_LABEL[module], \
    FORMATTED(__VA_ARGS__))
#define FACEAUTH_HILOGI(module, ...) (void)OHOS::HiviewDFX::HiLog::Info(FACE_AUTH_LABEL[module], \
    FORMATTED(__VA_ARGS__))
#define FACEAUTH_HILOGD(module, ...) (void)OHOS::HiviewDFX::HiLog::Debug(FACE_AUTH_LABEL[module], \
    FORMATTED(__VA_ARGS__))
const uint64_t MASK = 0xffff;
const size_t MASKED_STRING_LEN = 9;
inline std::string getMaskedString(uint64_t val)
{
    char bytes[MASKED_STRING_LEN] = {0};
    if (snprintf_s(bytes, sizeof(bytes), sizeof(bytes) - 1, "xxxx%04" PRIx64, val & MASK) == 0) {
        return "(snprintf fail)";
    }
    return std::string(bytes);
}

inline std::string getPointerNullString(void *p)
{
    if (p == nullptr) {
        return "null";
    }
    return "non-null";
}
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS
#endif // CONFIG_HILOG
#endif  // FACE_AUTH_LOG_WRAPPER_H