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

#ifndef FACE_AUTH_EXECUTOR_CALLBACK_HDI
#define FACE_AUTH_EXECUTOR_CALLBACK_HDI

#include <cstdint>

#include "hdf_base.h"
#include "nocopyable.h"

#include "face_auth_executor_hdi.h"
#include "face_auth_hdi.h"
#include "iam_executor_iexecute_callback.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
class FaceAuthExecutorCallbackHdi : public IExecutorCallback, public NoCopyable {
public:
    explicit FaceAuthExecutorCallbackHdi(std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback);
    ~FaceAuthExecutorCallbackHdi() override = default;

    // IExecutorCallback
    int32_t OnResult(int32_t result, const std::vector<uint8_t> &extraInfo) override;
    int32_t OnTip(int32_t tip, const std::vector<uint8_t> &extraInfo) override;

private:
    UserIam::UserAuth::ResultCode ConvertResultCode(const int32_t in);

    std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_EXECUTOR_CALLBACK_HDI
