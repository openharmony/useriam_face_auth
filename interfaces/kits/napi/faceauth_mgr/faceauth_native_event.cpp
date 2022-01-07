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

#include "faceauth_native_event.h"

#include "faceauth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthNativeEvent::FaceAuthNativeEvent(napi_env env, napi_value thisVar) : thisVarRef_(nullptr)
{
    napi_create_reference(env, thisVar, 1, &thisVarRef_);
}

FaceAuthNativeEvent::~FaceAuthNativeEvent()
{
    for (auto iter = eventMap_.begin(); iter != eventMap_.end(); ++iter) {
        auto listener = iter->second;
        napi_delete_reference(env_, listener->handlerRef);
    }
    eventMap_.clear();
    napi_delete_reference(env_, thisVarRef_);
}

void FaceAuthNativeEvent::On(std::string &eventType, napi_value handler)
{
    FACEAUTH_LABEL_LOGI("FaceAuthNativeEvent On in for event: %{public}s", eventType.c_str());
    auto listener = std::make_shared<FaceAuthEventListener>();
    listener->eventType = eventType;
    napi_create_reference(env_, handler, 1, &listener->handlerRef);
    eventMap_[eventType] = listener;
}

int32_t FaceAuthNativeEvent::Off(std::string &eventType)
{
    FACEAUTH_LABEL_LOGI("FaceAuthNativeEvent Off in for event: %{public}s", eventType.c_str());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        FACEAUTH_LABEL_LOGI("scope is nullptr");
        return Constant::JS_FAILURE;
    }

    auto iter = eventMap_.find(eventType);
    if (iter == eventMap_.end()) {
        FACEAUTH_LABEL_LOGI("eventType %{public}s not find", eventType.c_str());
        return Constant::JS_FAILURE;
    }
    auto listener = iter->second;
    napi_delete_reference(env_, listener->handlerRef);
    eventMap_.erase(eventType);
    napi_close_handle_scope(env_, scope);
    return Constant::SUCCESS;
}

void FaceAuthNativeEvent::OnEvent(const std::string &eventType, size_t argc, const napi_value *argv)
{
    FACEAUTH_LABEL_LOGI("OnEvent for %{public}s", eventType.c_str());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        FACEAUTH_LABEL_LOGI("scope is nullptr");
        return;
    }

    auto iter = eventMap_.find(eventType);
    if (iter == eventMap_.end()) {
        FACEAUTH_LABEL_LOGI("eventType %{public}s not find", eventType.c_str());
        return;
    }
    auto listener = iter->second;
    napi_value thisVar = nullptr;
    napi_status status = napi_get_reference_value(env_, thisVarRef_, &thisVar);
    if (status != napi_ok) {
        FACEAUTH_LABEL_LOGI(
            "napi_get_reference_value thisVar for %{public}s failed, status=%{public}d", eventType.c_str(), status);
        return;
    }

    napi_value handler = nullptr;
    status = napi_get_reference_value(env_, listener->handlerRef, &handler);
    if (status != napi_ok) {
        FACEAUTH_LABEL_LOGI(
            "napi_get_reference_value handler for %{public}s failed, status=%{public}d", eventType.c_str(), status);
        return;
    }

    napi_value callResult = nullptr;
    status = napi_call_function(env_, thisVar, handler, argc, argv, &callResult);
    if (status != napi_ok) {
        FACEAUTH_LABEL_LOGI("napi_call_function for %{public}s failed, status=%{public}d", eventType.c_str(), status);
        return;
    }
    napi_close_handle_scope(env_, scope);
}
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS
