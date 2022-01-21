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

#include "face_auth_stub.h"
#include "faceauth_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthStub::FaceAuthStub()
{
    FACEAUTH_LABEL_LOGI("enter");
    MyKeyToHandle();
}

FaceAuthStub::~FaceAuthStub()
{
    FACEAUTH_LABEL_LOGI("enter");
}

void FaceAuthStub::MyKeyToHandle()
{
    m_KeyToHandle_[FACE_AUTH_AUTHENTICATE] = &FaceAuthStub::FaceAuthAuthenticate;
    m_KeyToHandle_[FACE_AUTH_CANCEL_AUTH] = &FaceAuthStub::FaceAuthCancelAuth;
    m_KeyToHandle_[FACE_AUTH_INIT] = &FaceAuthStub::FaceAuthInit;
    m_KeyToHandle_[FACE_AUTH_RELEASE] = &FaceAuthStub::FaceAuthRelease;
    m_KeyToHandle_[FACE_AUTH_RESET_TIMEOUT] = &FaceAuthStub::FaceAuthResetTimeout;
    m_KeyToHandle_[FACE_AUTH_GET_REMAINING_NUM] = &FaceAuthStub::FaceAuthGetRemainingNum;
    m_KeyToHandle_[FACE_AUTH_GET_REMAINING_TIME] = &FaceAuthStub::FaceAuthGetRemainingTime;
    m_KeyToHandle_[FACE_AUTH_PREENROLL] = &FaceAuthStub::FaceAuthPreEnroll;
    m_KeyToHandle_[FACE_AUTH_POSTENROLL] = &FaceAuthStub::FaceAuthPostEnroll;
    m_KeyToHandle_[FACE_AUTH_REMOVE] = &FaceAuthStub::FaceAuthRemove;
    m_KeyToHandle_[FACE_AUTH_GET_ENROLLED_FACEIDS] = &FaceAuthStub::FaceAuthGetEnrolledFaceIds;
    m_KeyToHandle_[FACE_AUTH_GET_ANGLEDIM] = &FaceAuthStub::FaceAuthGetAngledim;
    m_KeyToHandle_[FACE_AUTH_ENROLL] = &FaceAuthStub::FaceAuthEnroll;
    m_KeyToHandle_[FACE_AUTH_CANCEL_ENROLLMENT] = &FaceAuthStub::FaceAuthCancelEnrollment;
}

int32_t FaceAuthStub::FaceAuthAuthenticate(MessageParcel &data, MessageParcel &reply)
{
    AuthParam param;
    param.reqId = data.ReadUint64();
    param.flags = data.ReadInt32();
    param.previewId = data.ReadInt32();
    param.challenge = data.ReadInt64();
    param.faceId = data.ReadInt32();
    sptr<OnFaceAuth> callback = iface_cast<OnFaceAuth>(data.ReadRemoteObject());
    if (callback == nullptr) {
        return FA_RET_ERROR;
    }
    int32_t ret = Authenticate(param, callback);
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthCancelAuth(MessageParcel &data, MessageParcel &reply)
{
    uint64_t reqId = data.ReadUint64();
    sptr<OnFaceAuth> callback = iface_cast<OnFaceAuth>(data.ReadRemoteObject());
    if (callback == nullptr) {
        return FA_RET_ERROR;
    }
    int32_t ret = CancelAuth(reqId, callback);
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthInit(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = Init();
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_OK;
    }
    return FA_RET_OK;
}
int32_t FaceAuthStub::FaceAuthRelease(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = Release();
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthResetTimeout(MessageParcel &data, MessageParcel &reply)
{
    ResetTimeout();
    reply.WriteInt32(0);
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthGetRemainingNum(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = GetRemainingNum();
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthGetRemainingTime(MessageParcel &data, MessageParcel &reply)
{
    int64_t ret = GetRemainingTime();
    if (!reply.WriteInt64(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt64(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthPreEnroll(MessageParcel &data, MessageParcel &reply)
{
    int64_t ret = PreEnroll();
    if (!reply.WriteInt64(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthPostEnroll(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = PostEnroll();
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthRemove(MessageParcel &data, MessageParcel &reply)
{
    RemoveParam param;
    param.reqId = data.ReadUint64();
    param.faceId = data.ReadInt32();
    sptr<OnFaceAuth> callback = iface_cast<OnFaceAuth>(data.ReadRemoteObject());
    if (callback == nullptr) {
        return FA_RET_ERROR;
    }
    int32_t ret = Remove(param, callback);
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthGetEnrolledFaceIds(MessageParcel &data, MessageParcel &reply)
{
    int32_t type = data.ReadInt32();
    std::list<int32_t> ret = GetEnrolledFaceIDs(type);
    reply.WriteInt32(ret.size());
    for (auto stack : ret) {
        reply.WriteInt32(stack);
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthGetAngledim(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = GetAngleDim();
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthEnroll(MessageParcel &data, MessageParcel &reply)
{
    EnrollParam param;
    param.reqId = data.ReadUint64();
    param.previewId = data.ReadInt32();
    param.challenge = data.ReadInt64();
    param.faceId = data.ReadInt32();
    data.ReadUInt8Vector(&param.token);
    for (std::vector<uint8_t>::const_iterator iter = param.token.begin(); iter != param.token.end(); ++iter) {
        FACEAUTH_LABEL_LOGI("stub param.token is %{public}d", *iter);
    }
    sptr<OnFaceAuth> callback = iface_cast<OnFaceAuth>(data.ReadRemoteObject());
    if (callback == nullptr) {
        return FA_RET_ERROR;
    }
    int32_t ret = Enroll(param, callback);
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::FaceAuthCancelEnrollment(MessageParcel &data, MessageParcel &reply)
{
    uint64_t reqId = data.ReadUint64();
    sptr<OnFaceAuth> callback = iface_cast<OnFaceAuth>(data.ReadRemoteObject());
    if (callback == nullptr) {
        return FA_RET_ERROR;
    }
    int32_t ret = CancelEnrollment(reqId, callback);
    if (!reply.WriteInt32(ret)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(ret)");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    FACEAUTH_LABEL_LOGI("enter");
    FACEAUTH_LABEL_LOGI("code = %{public}d", code);

    auto itFunc = m_KeyToHandle_.find(code);
    if (itFunc != m_KeyToHandle_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    FACEAUTH_LABEL_LOGI("AbilitySchedulerStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS