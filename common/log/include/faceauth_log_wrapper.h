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

#ifndef FACEAUTH_LOG_WRAPPER_H
#define FACEAUTH_LOG_WRAPPER_H

#include <string>
#include "hilog/log.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
#ifndef LOG_DOMAIN
#define LOG_DOMAIN 0xD002422
#endif
#ifndef APP_LOG_TAG
#define APP_LOG_TAG NULL
#endif

enum class FaceAuthLogLevel { DEBUG = 0, INFO, WARN, ERROR, FATAL };

static constexpr OHOS::HiviewDFX::HiLogLabel FACEAUTH_LABEL = {LOG_CORE, LOG_DOMAIN, APP_LOG_TAG};

class FaceAuthLogWrapper {
public:
    static bool JudgeLevel(const FaceAuthLogLevel &level);

    static void SetLogLevel(const FaceAuthLogLevel &level)
    {
        level_ = level;
    }

    static const FaceAuthLogLevel &GetLogLevel()
    {
        return level_;
    }

    static std::string GetBriefFileName(const char *str);

private:
    static FaceAuthLogLevel level_;
};

#define PRINT_LOG(LEVEL, Level, fmt, ...)                        \
    if (FaceAuthLogWrapper::JudgeLevel(FaceAuthLogLevel::LEVEL)) \
    OHOS::HiviewDFX::HiLog::Level(FACEAUTH_LABEL,                \
        "[%{public}s:%{public}d(%{public}s)] " fmt,              \
        FaceAuthLogWrapper::GetBriefFileName(__FILE__).c_str(),  \
        __LINE__,                                                \
        __FUNCTION__,                                            \
        ##__VA_ARGS__)

#define FACEAUTH_LABEL_LOGI(fmt, ...) PRINT_LOG(INFO, Info, fmt, ##__VA_ARGS__)
#define FACEAUTH_LABEL_LOGW(fmt, ...) PRINT_LOG(WARN, Warn, fmt, ##__VA_ARGS__)
#define FACEAUTH_LABEL_LOGE(fmt, ...) PRINT_LOG(ERROR, Error, fmt, ##__VA_ARGS__)
#define FACEAUTH_LABEL_LOGF(fmt, ...) PRINT_LOG(FATAL, Fatal, fmt, ##__VA_ARGS__)
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif  // FACEAUTH_LOG_WRAPPER_H