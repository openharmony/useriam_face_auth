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

#include "face_auth_algo_dispatcher.h"
#include <future>
#include "faceauth_log_wrapper.h"
#include "face_auth_ca.h"
#include "face_auth_defines.h"
#include "face_auth_thread_pool.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthAlgoDispatcher::mutex_;
std::shared_ptr<FaceAuthAlgoDispatcher> FaceAuthAlgoDispatcher::instance_ = nullptr;
std::shared_ptr<FaceAuthAlgoDispatcher> FaceAuthAlgoDispatcher::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthAlgoDispatcher>();
        }
    }
    return instance_;
}

FaceAuthAlgoDispatcher::FaceAuthAlgoDispatcher()
{
    bundleNameList_.clear();
}

FaceAuthAlgoDispatcher::~FaceAuthAlgoDispatcher()
{}

AlgoResult FaceAuthAlgoDispatcher::IsNeedAlgoLoad(std::string bundleName)
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoDispatcher IsNeedAlgoLoad start");
    AlgoResult result = AR_SUCCESS;

    if (bundleNameList_.empty()) {
        result = AR_EMPTY;
    }

    if (bundleNameList_.find(bundleName) != bundleNameList_.end()) {
        bundleNameList_[bundleName] = bundleNameList_[bundleName] + 1;
        FACEAUTH_LABEL_LOGI(
            "Add same bundleName:%{public}s,num:%{public}d", bundleName.c_str(), bundleNameList_[bundleName]);
        return AR_ADD_AGAIN;
    }

    bundleNameList_.insert(std::pair<std::string, int32_t>(bundleName, 1));

    FACEAUTH_LABEL_LOGI("Insert bundleName:%{public}s", bundleName.c_str());
    FACEAUTH_LABEL_LOGI("Result:%{public}d", result);
    return result;
}

AlgoResult FaceAuthAlgoDispatcher::IsNeedAlgoRelease(std::string bundleName)
{
    AlgoResult result = AR_SUCCESS;
    if (bundleNameList_.erase(bundleName) != 0) {
        if (bundleNameList_.empty()) {
            result = AR_EMPTY;
        }
        FACEAUTH_LABEL_LOGI("Remove Success bundleName:%{public}s", bundleName.c_str());
    } else {
        result = AR_NOT_FOUND;
        FACEAUTH_LABEL_LOGE("Remove Fail bundleName:%{public}s", bundleName.c_str());
    }
    return result;
}

FIRetCode FaceAuthAlgoDispatcher::Init(std::string bundleName)
{
    FACEAUTH_LABEL_LOGI("Init ,bundleName:%{public}s", bundleName.c_str());
    AlgoResult result = IsNeedAlgoLoad(bundleName);
    if (result == AR_EMPTY) {
        std::promise<int32_t> promiseobj;
        std::future<int32_t> futureobj = promiseobj.get_future();
        FaceAuthThreadPool::GetInstance()->AddTask(
            [&promiseobj]() { promiseobj.set_value(FaceAuthAlgoAdapter::GetInstance()->DynamicInit()); });
        std::chrono::microseconds span(INIT_DYNAMIC_TIME_OUT);
        while (futureobj.wait_for(span) == std::future_status::timeout) {
            FACEAUTH_LABEL_LOGI("Init Dynamic TimeOut");
            return FI_RC_ERROR;
        }
        return static_cast<FIRetCode>(futureobj.get());
    }
    FACEAUTH_LABEL_LOGE("Init Fail %{public}d", result);
    return FI_RC_ERROR;
}

bool FaceAuthAlgoDispatcher::IsInited()
{
    if (bundleNameList_.empty()) {
        FACEAUTH_LABEL_LOGI("FaceAuthAlgoDispatcher IsInited bundleNameList_ is empty, need init");
        return true;
    }
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoDispatcher IsInited bundleNameList_ is not empty,no need init");
    return false;
}

FIRetCode FaceAuthAlgoDispatcher::GetEnrollPoseCount(int32_t &postCount)
{
    return FaceAuthAlgoAdapter::GetInstance()->GetEnrollPoseCount(postCount);
}

FIRetCode FaceAuthAlgoDispatcher::Prepare(HWExeType type, std::unique_ptr<PrepareParam> param)
{
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoDispatcher::Prepare start");
    if (type < HW_EXEC_TYPE_ENROOL || type > HW_EXEC_TYPE_LEARN) {
        FACEAUTH_LABEL_LOGE("Parameter check error.type is %{public}d", type);
        return FI_RC_INVALID_ARGUMENT;
    }
    if (param == nullptr) {
        FACEAUTH_LABEL_LOGE("Parameter check error.param is null");
        return FI_RC_NULLPOINTER;
    }
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoDispatcher::Prepare end");
    return FaceAuthAlgoAdapter::GetInstance()->Prepare(type, std::move(param));
}

FIRetCode FaceAuthAlgoDispatcher::VerifyAuthToken(std::unique_ptr<uint8_t[]> authToken, uint32_t length, int32_t type)
{
    int32_t result = FaceAuthCa::GetInstance()->VerifyAuthToken(std::move(authToken), length, type);
    if (result == 0) {
        return FI_RC_OK;
    }
    return FI_RC_ERROR;
}

FIRetCode FaceAuthAlgoDispatcher::TransformImage(std::unique_ptr<CameraImageStream> img, int32_t count)
{
    if (count < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.count is %{public}d", count);
        return FI_RC_INVALID_ARGUMENT;
    }
    if (img == nullptr) {
        FACEAUTH_LABEL_LOGE("Parameter check error.img is null");
        return FI_RC_NULLPOINTER;
    }
    return FaceAuthAlgoAdapter::GetInstance()->TransformImage(std::move(img), count);
}
FIRetCode FaceAuthAlgoDispatcher::GetResult(int32_t &resultCode, int32_t param[RESULT_MAX_SIZE])
{
    return FaceAuthAlgoAdapter::GetInstance()->GetResult(resultCode, param);
}

FIRetCode FaceAuthAlgoDispatcher::Reset(uint32_t errorCode)
{
    if (errorCode < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.errorCode is %{public}d", errorCode);
        return FI_RC_INVALID_ARGUMENT;
    }
    return FaceAuthAlgoAdapter::GetInstance()->Reset(errorCode);
}

FIRetCode FaceAuthAlgoDispatcher::PreEnroll(int64_t &challenge, int32_t type)
{
    int32_t result = FaceAuthCa::GetInstance()->PreEnroll(challenge, type);
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoDispatcher PreEnroll result:%{public}d", result);
    if (result == 0) {
        return FI_RC_OK;
    }
    return FI_RC_ERROR;
}

FIRetCode FaceAuthAlgoDispatcher::PostEnroll(int32_t type)
{
    int32_t result = FaceAuthCa::GetInstance()->PostEnroll(type);
    FACEAUTH_LABEL_LOGI("FaceAuthAlgoDispatcher PostEnroll result:%{public}d", result);
    if (result == 0) {
        return FI_RC_OK;
    }
    return FI_RC_ERROR;
}

FIRetCode FaceAuthAlgoDispatcher::Release(std::string bundleName)
{
    FACEAUTH_LABEL_LOGI("Release ,bundleName:%{public}s", bundleName.c_str());
    AlgoResult result = IsNeedAlgoRelease(bundleName);
    if (result == AR_EMPTY) {
        return FaceAuthAlgoAdapter::GetInstance()->DynamicRelease();
    }
    FACEAUTH_LABEL_LOGE("Release Fail %{public}d", result);
    return FI_RC_ERROR;
}

FIRetCode FaceAuthAlgoDispatcher::SetChallenge(int64_t challenge)
{
    if (challenge < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.challenge is %{private}lld", challenge);
        return FI_RC_INVALID_ARGUMENT;
    }
    int32_t result = FaceAuthCa::GetInstance()->SetChallenge(challenge);
    if (result == 0) {
        return FI_RC_OK;
    }
    return FI_RC_ERROR;
}

FIRetCode FaceAuthAlgoDispatcher::Cancel(uint64_t reqId)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    return FaceAuthAlgoAdapter::GetInstance()->Cancel(reqId);
}

FIRetCode FaceAuthAlgoDispatcher::GetEnrollResult(int32_t &authErrorCode, FICode &code, uint64_t reqId)
{
    FACEAUTH_LABEL_LOGI("GetEnrollResult");
    return FaceAuthAlgoAdapter::GetInstance()->GetEnrollResult(authErrorCode, code, reqId);
}

FIRetCode FaceAuthAlgoDispatcher::GetAuthResult(int32_t &authErrorCode, FICode &code, uint64_t reqId)
{
    FACEAUTH_LABEL_LOGI("GetAuthResult");
    return FaceAuthAlgoAdapter::GetInstance()->GetAuthResult(authErrorCode, code, reqId);
}

int32_t FaceAuthAlgoDispatcher::SetActiveGroup(const uint32_t uid, std::list<int32_t> &faceInfolist)
{
    return FaceAuthCa::GetInstance()->SetActiveGroup(uid, faceInfolist);
}

int32_t FaceAuthAlgoDispatcher::GetAuthToken(std::unique_ptr<uint8_t[]> &authToken, uint32_t &len)
{
    return FaceAuthCa::GetInstance()->GetAuthToken(authToken, len);
}

int32_t FaceAuthAlgoDispatcher::DeleteFace(int32_t faceId)
{
    return GetResultFromFIRetCode(FaceAuthAlgoAdapter::GetInstance()->DeleteFace(faceId));
}

int32_t FaceAuthAlgoDispatcher::GetResultFromFIRetCode(FIRetCode code)
{
    int32_t result = FA_RET_ERROR;
    if (code == 0) {
        result = FA_RET_OK;
    }
    return result;
}
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS