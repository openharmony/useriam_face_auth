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
#include "face_auth_thread_pool.h"
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthThreadPool::mutex_;
std::shared_ptr<FaceAuthThreadPool> FaceAuthThreadPool::instance_ = nullptr;
FaceAuthThreadPool::FaceAuthThreadPool()
{
    Start(FACE_AUTH_THREAD_NUM);
}

FaceAuthThreadPool::~FaceAuthThreadPool()
{
    Stop();
}

std::shared_ptr<FaceAuthThreadPool> FaceAuthThreadPool::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthThreadPool>();
        }
    }
    return instance_;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS