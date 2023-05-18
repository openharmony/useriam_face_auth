/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SERVICE_EX_MANAGER
#define SERVICE_EX_MANAGER

#include <mutex>

#include "timer.h"

#include "iam_common_defines.h"
#include "iscreen_brightness_task.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class ServiceExManager {
public:
    static ServiceExManager &GetInstance();

    UserAuth::ResultCode Acquire();
    void Release();
    void DoClose();
    std::shared_ptr<IScreenBrightnessTask> GetScreenBrightnessTask();

private:
    ServiceExManager() = default;
    ~ServiceExManager() = default;

    void *handle_;
    bool isOpened_ = false;
    std::recursive_mutex recursiveMutex_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // SERVICE_EX_MANAGER