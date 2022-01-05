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

#ifndef FACEAUTH_NATIVE_EVENT_H
#define FACEAUTH_NATIVE_EVENT_H
#include <map>
#include <memory>
#include <string>
#include "napi/native_api.h"
#include "constant.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
struct FaceAuthEventListener {
    std::string eventType;
    napi_ref handlerRef = nullptr;
};

class FaceAuthNativeEvent {
public:
    FaceAuthNativeEvent(napi_env env, napi_value thisVar);
    virtual ~FaceAuthNativeEvent();

    virtual void On(std::string &eventType, napi_value handler);
    virtual int32_t Off(std::string &eventType);
    virtual void OnEvent(const std::string &eventType, size_t argc, const napi_value *argv);

protected:
    static napi_env env_;
    napi_ref thisVarRef_;
    std::map<std::string, std::shared_ptr<FaceAuthEventListener>> eventMap_;
};
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS

#endif /* FACEAUTH_NATIVE_EVENT_H */