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

#ifndef MOCK_IEXECUTE_CALLBACK_H
#define MOCK_IEXECUTE_CALLBACK_H

#include "gmock/gmock.h"

#include "iam_executor_iexecute_callback.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class MockIExecuteCallback : public UserIam::UserAuth::IExecuteCallback {
public:
    MockIExecuteCallback() = default;
    virtual ~MockIExecuteCallback() = default;

    MOCK_METHOD2(OnResult, void(ResultCode result, const std::vector<uint8_t> &extraInfo));
    MOCK_METHOD1(OnResult, void(ResultCode result));
    MOCK_METHOD2(OnAcquireInfo, void(int32_t acquire, const std::vector<uint8_t> &extraInfo));
    MOCK_METHOD2(OnMessage, void(int destRole, const std::vector<uint8_t> &msg));
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS

#endif // MOCK_IEXECUTE_CALLBACK_H