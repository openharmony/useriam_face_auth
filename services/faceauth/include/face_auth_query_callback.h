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
#ifndef FACE_AUTH_QUERY_CALLBACK_H
#define FACE_AUTH_QUERY_CALLBACK_H
#include "face_auth_defines.h"
#include "query_callback.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthQueryCallback : public AuthResPool::QueryCallback {
public:
    FaceAuthQueryCallback()=default;
    virtual ~FaceAuthQueryCallback()=default;
    void OnResult(uint32_t resultCode) override;
};
} // namespace FaceAuth
} // namespace userIAM
} // namespace OHOS
#endif // FACE_AUTH_QUERY_CALLBACK_H
