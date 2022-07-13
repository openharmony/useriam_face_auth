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

#include "face_auth_manager.h"

#include <cstddef>
#include <cstdint>
#include <istream>
#include <sstream>
#include <string>

#include "ibuffer_producer.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "napi/native_common.h"
#include "refbase.h"
#include "surface.h"
#include "surface_utils.h"

#include "iam_check.h"
#include "iam_logger.h"

#include "face_auth_defines.h"
#include "face_auth_innerkit.h"

#define LOG_LABEL UserIAM::Common::LABEL_FACE_AUTH_NAPI

using namespace std;

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace {
FaceAuthRet GetOneParaArgv(napi_env env, napi_callback_info info, napi_value &argv)
{
    size_t argc = 1;
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr), FACEAUTH_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(argc == 1, FACEAUTH_ERROR);
    return FACEAUTH_SUCCESS;
}

string GetUint64StringFromNapiPara(napi_env env, napi_value para)
{
    constexpr int uint64StrMaxLen = 25;
    char buf[uint64StrMaxLen + 1] = {0};
    size_t len = 0;
    napi_get_value_string_utf8(env, para, buf, uint64StrMaxLen, &len);
    buf[len] = 0;
    return string(buf);
}

uint64_t GetUint64FromString(const string &in)
{
    istringstream surfaceIdStream(in);
    uint64_t surfaceId = 0;
    surfaceIdStream >> surfaceId;
    return surfaceId;
}

FaceAuthRet GetBufferProducerBySurfaceId(uint64_t surfaceId, sptr<IBufferProducer> &bufferProducer)
{
    if (surfaceId == 0) {
        IAM_LOGI("surface id is 0, get buffer producer null");
        bufferProducer = nullptr;
        return FACEAUTH_SUCCESS;
    }

    auto surfaceUtils = SurfaceUtils::GetInstance();
    if (surfaceUtils == nullptr) {
        IAM_LOGE("Get SurfaceUtils failed!");
        return FACEAUTH_ERROR;
    }

    sptr<Surface> previewSurface = surfaceUtils->GetSurface(surfaceId);
    if (previewSurface == nullptr) {
        IAM_LOGE("GetSurface failed!");
        return FACEAUTH_ERROR;
    }

    bufferProducer = previewSurface->GetProducer();
    if (bufferProducer == nullptr) {
        IAM_LOGE("GetProducer Failed!");
        return FACEAUTH_ERROR;
    }

    IAM_LOGI("get buffer producer success");
    return FACEAUTH_SUCCESS;
}
} // namespace

FaceAuthRet FaceAuthManager::SetSurfaceId(napi_env env, napi_callback_info info)
{
    napi_value argv;
    auto getArgvRet = GetOneParaArgv(env, info, argv);
    IF_FALSE_LOGE_AND_RETURN_VAL(getArgvRet == FACEAUTH_SUCCESS, FACEAUTH_ERROR);

    auto uint64String = GetUint64StringFromNapiPara(env, argv);
    uint64_t surfaceId = GetUint64FromString(uint64String);

    sptr<IBufferProducer> bufferProducer = nullptr;
    auto getBufferProducerResult = GetBufferProducerBySurfaceId(surfaceId, bufferProducer);
    IF_FALSE_LOGE_AND_RETURN_VAL(getBufferProducerResult == FACEAUTH_SUCCESS, FACEAUTH_ERROR);

    auto setBufferProducerResult = FaceAuthInnerKit::SetBufferProducer(bufferProducer);
    IF_FALSE_LOGE_AND_RETURN_VAL(setBufferProducerResult == FACEAUTH_SUCCESS, FACEAUTH_ERROR);
    return FACEAUTH_SUCCESS;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS