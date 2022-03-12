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

#include "face_auth_req.h"
#include "face_auth_log_wrapper.h"
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthReq::mutex_;
std::shared_ptr<FaceAuthReq> FaceAuthReq::instance_ = nullptr;
FaceAuthReq::FaceAuthReq()
{}

FaceAuthReq::~FaceAuthReq()
{
    reqInfoList_.clear();
}

std::shared_ptr<FaceAuthReq> FaceAuthReq::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthReq>();
        }
    }
    return instance_;
}

bool FaceAuthReq::IsReqNumReachedMax(FaceOperateType type)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    PrintReqInfoList();
    FACEAUTH_HILOGI(MODULE_SERVICE, "type is %{public}d", type);
    if (type > FACE_OPERATE_TYPE_MAX || type <= FACE_INVALID_OPERATE_TYPE) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Parameter check error.type is %{public}d", type);
        return true;
    }
    if (type == FACE_OPERATE_TYPE_CO_AUTH) {
        int32_t count = 0;
        for (auto iter = reqInfoList_.begin(); iter != reqInfoList_.end(); ++iter) {
            if (iter->first.operateType == type) {
                count++;
            }
        }
        if (count >= CO_AUTH_MAX_NUM - 1) {
            return true;
        }
    } else {
        for (auto iter = reqInfoList_.begin(); iter != reqInfoList_.end(); ++iter) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "operateType is %{public}d", iter->first.operateType);
            if (iter->first.operateType == type) {
                FACEAUTH_HILOGI(MODULE_SERVICE, "reqId is xxxx%04llu", iter->first.reqId);
                return true;
            }
        }
    }
    return false;
}

void FaceAuthReq::AddReqInfo(FaceReqType reqType, FaceInfo reqInfo)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_HILOGI(MODULE_SERVICE, "reqType.reqId is xxxx%04llu,"
                                    "reqType.operateType is %{public}d,"
                                    "reqInfo.eventId is %{public}u,"
                                    "reqInfo.uId is %{public}d,"
                                    "reqInfo.isCanceled is %{public}d",
                    reqType.reqId, reqType.operateType, reqInfo.eventId, reqInfo.uId, reqInfo.isCanceled);
    if (reqType.operateType <= FACE_INVALID_OPERATE_TYPE || reqType.operateType > FACE_OPERATE_TYPE_MAX) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Parameter check error.reqInfo.operateType is %{public}d", reqType.operateType);
        return;
    }
    auto req2Rm = reqInfoList_.find(reqType);
    if (req2Rm != reqInfoList_.end()) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "same key was found.");
    } else {
        reqInfoList_.insert(std::pair<FaceReqType, FaceInfo>(reqType, reqInfo));
    }
    return;
}

void FaceAuthReq::RemoveRequireInfo(FaceReqType reqType)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    PrintReqInfoList();
    FACEAUTH_HILOGI(MODULE_SERVICE, "Remove reqType.reqId is xxxx%04llu,"
                                    "Remove reqType.operateType is %{public}d", reqType.reqId, reqType.operateType);
    auto req2Rm = reqInfoList_.find(reqType);
    if (req2Rm != reqInfoList_.end()) {
        reqInfoList_.erase(reqType);
    } else {
        FACEAUTH_HILOGI(MODULE_SERVICE, "no require info found.");
    }
    return;
}

void FaceAuthReq::RemoveAllRequireInfo()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    reqInfoList_.clear();
}

FaceOperateType FaceAuthReq::GetOperateType(uint32_t eventId)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_HILOGI(MODULE_SERVICE, "eventId is %{public}u", eventId);
    for (auto iter = reqInfoList_.begin(); iter != reqInfoList_.end(); ++iter) {
        if (iter->second.eventId == eventId) {
            return iter->first.operateType;
        }
    }
    return FACE_INVALID_OPERATE_TYPE;
}

uint32_t FaceAuthReq::GetEventId(FaceReqType reqType)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_HILOGI(MODULE_SERVICE, "reqType.reqId is xxxx%04llu", reqType.reqId);
    FACEAUTH_HILOGI(MODULE_SERVICE, "reqType.operateType is %{public}d", reqType.operateType);
    auto iter = reqInfoList_.find(reqType);
    if (iter != reqInfoList_.end()) {
        return reqInfoList_[reqType].eventId;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "reqType is not in reqInfoList, reqType.reqId is xxxx%04llu", reqType.reqId);
    return 0;
}

bool FaceAuthReq::SetCancelFlagSuccess(FaceReqType reqType, int32_t uId)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_HILOGI(MODULE_SERVICE, "reqType.reqId is xxxx%04llu", reqType.reqId);
    FACEAUTH_HILOGI(MODULE_SERVICE, "reqType.operateType is %{public}d", reqType.operateType);
    FACEAUTH_HILOGI(MODULE_SERVICE, "uId is %{public}d", uId);
    auto iter = reqInfoList_.find(reqType);
    if (iter != reqInfoList_.end() && uId == reqInfoList_[reqType].uId) {
        reqInfoList_[reqType].isCanceled = true;
        return true;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE,
        "reqType or uId is not in reqInfoList, reqType.reqId is xxxx%04llu, uid is%{public}d", reqType.reqId, uId);
    return false;
}

bool FaceAuthReq::isCanceled(uint32_t eventId, int32_t uId)
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    FACEAUTH_HILOGI(MODULE_SERVICE, "eventId is %{public}u", eventId);
    FACEAUTH_HILOGI(MODULE_SERVICE, "uId is %{public}d", uId);
    for (auto iter = reqInfoList_.begin(); iter != reqInfoList_.end(); ++iter) {
        if (iter->second.eventId == eventId && uId == iter->second.uId) {
            return iter->second.isCanceled;
        }
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "eventId or uId is not in reqInfoList, eventId is %{public}u, uid is%{public}d",
        eventId, uId);
    return false;
}

bool FaceAuthReq::FindLocalAuth()
{
    std::lock_guard<std::mutex> lock_l(mutex_);
    bool result = false;
    for (auto iter = reqInfoList_.begin(); iter != reqInfoList_.end(); ++iter) {
        if (iter->first.operateType == FACE_OPERATE_TYPE_LOCAL_AUTH) {
            result = true;
        }
    }
    return result;
}

void FaceAuthReq::PrintReqInfoList()
{
    for (auto iterinfo = reqInfoList_.begin(); iterinfo != reqInfoList_.end(); ++iterinfo) {
        FACEAUTH_HILOGI(MODULE_SERVICE,
            "ListInfo:reqId:xxxx%04llu, Type:%{public}d, eventId:%{public}u, uId:%{public}d, isCanceled:%{public}d",
            iterinfo->first.reqId, iterinfo->first.operateType, iterinfo->second.eventId, iterinfo->second.uId,
            iterinfo->second.isCanceled);
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
