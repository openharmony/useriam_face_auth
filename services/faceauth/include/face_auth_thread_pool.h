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

#ifndef FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_THREAD_POOL_H
#define FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_THREAD_POOL_H
#include <mutex>
#include "nocopyable.h"
#include "thread_pool.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthThreadPool : public ThreadPool {
public:
    FaceAuthThreadPool();
    virtual ~FaceAuthThreadPool();
    static std::shared_ptr<FaceAuthThreadPool> GetInstance();

private:
    static std::mutex mutex_;
    static std::shared_ptr<FaceAuthThreadPool> instance_;
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_FACE_AUTH_THREAD_POOL_H
