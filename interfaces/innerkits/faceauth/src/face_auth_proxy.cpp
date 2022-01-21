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

#include "face_auth_proxy.h"
#include "faceauth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
FaceAuthProxy::FaceAuthProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IFaceAuth>(object)
{
    FACEAUTH_LABEL_LOGI("enter");
}

FaceAuthProxy::~FaceAuthProxy()
{
    FACEAUTH_LABEL_LOGI("enter");
}

int32_t FaceAuthProxy::Init()
{
    MessageParcel data;
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_INIT, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int32_t FaceAuthProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_RELEASE, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int32_t FaceAuthProxy::Enroll(const EnrollParam &param, const sptr<OnFaceAuth> &callback)
{
    MessageParcel data;
    if (!data.WriteUint64(param.reqId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(reqId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt32(param.previewId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(previewId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt64(param.challenge)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt64(challenge).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt32(param.faceId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(faceId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteUInt8Vector(param.token)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(token).");
        return FA_RET_ERROR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        FACEAUTH_LABEL_LOGE("failed to WriteRemoteObject(callback).");
        return FA_RET_ERROR;
    }
    for (std::vector<uint8_t>::const_iterator iter = param.token.begin(); iter != param.token.end(); ++iter) {
        FACEAUTH_LABEL_LOGI("proxy param.token is %{public}d", *iter);
    }
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_ENROLL, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int32_t FaceAuthProxy::Authenticate(const AuthParam &param, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId = xxxx%04llu, flags = %{public}d, challenge = %{private}lld, faceId = %{private}d",
        param.reqId, param.flags, param.challenge, param.faceId);
    MessageParcel data;
    if (!data.WriteUint64(param.reqId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(reqId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt32(param.flags)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(flags).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt32(param.previewId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(previewId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt64(param.challenge)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt64(challenge).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt32(param.faceId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(faceId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        FACEAUTH_LABEL_LOGE("failed to WriteRemoteObject(callback).");
        return FA_RET_ERROR;
    }
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_AUTHENTICATE, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int32_t FaceAuthProxy::CancelEnrollment(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    MessageParcel data;
    if (!data.WriteUint64(reqId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(reqId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        FACEAUTH_LABEL_LOGE("failed to WriteRemoteObject(callback).");
        return FA_RET_ERROR;
    }
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_CANCEL_ENROLLMENT, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int32_t FaceAuthProxy::CancelAuth(const uint64_t reqId, const sptr<OnFaceAuth> &callback)
{
    FACEAUTH_LABEL_LOGI("reqId = xxxx%04llu", reqId);
    MessageParcel data;
    if (!data.WriteUint64(reqId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(reqId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        FACEAUTH_LABEL_LOGE("failed to WriteRemoteObject(callback).");
        return FA_RET_ERROR;
    }
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_CANCEL_AUTH, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int64_t FaceAuthProxy::PreEnroll()
{
    MessageParcel data;
    MessageParcel reply;
    int64_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_PREENROLL, data, reply);
    if (ret) {
        result = reply.ReadInt64();
        FACEAUTH_LABEL_LOGI("result = %{public}lld", result);
    }
    return result;
}

int32_t FaceAuthProxy::PostEnroll()
{
    MessageParcel data;
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_POSTENROLL, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int32_t FaceAuthProxy::Remove(const RemoveParam &param, const sptr<OnFaceAuth> &callback)
{
    MessageParcel data;
    if (!data.WriteUint64(param.reqId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(reqId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteInt32(param.faceId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(faceId).");
        return FA_RET_ERROR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        FACEAUTH_LABEL_LOGE("failed to WriteRemoteObject(callback).");
        return FA_RET_ERROR;
    }
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_REMOVE, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

std::list<int32_t> FaceAuthProxy::GetEnrolledFaceIDs(const int32_t userId)
{
    std::list<int32_t> result;
    MessageParcel data;
    if (!data.WriteInt32(userId)) {
        FACEAUTH_LABEL_LOGE("failed to WriteInt32(challenge).");
        return result;
    }

    MessageParcel reply;
    bool ret = SendRequest(FACE_AUTH_GET_ENROLLED_FACEIDS, data, reply);
    if (ret) {
        int32_t num = reply.ReadInt32();
        for (int32_t i = 0; i < num; i++) {
            result.push_back(reply.ReadInt32());
        }
    }
    return result;
}

void FaceAuthProxy::ResetTimeout()
{
    MessageParcel data;
    MessageParcel reply;
    bool ret = SendRequest(FACE_AUTH_RESET_TIMEOUT, data, reply);
    if (ret) {
        int32_t result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return;
}

int32_t FaceAuthProxy::GetRemainingNum()
{
    MessageParcel data;
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_GET_REMAINING_NUM, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

int64_t FaceAuthProxy::GetRemainingTime()
{
    MessageParcel data;
    MessageParcel reply;
    int64_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_GET_REMAINING_TIME, data, reply);
    if (ret) {
        result = reply.ReadInt64();
        FACEAUTH_LABEL_LOGI("result = %{public}lld", result);
    }
    return result;
}

int32_t FaceAuthProxy::GetAngleDim()
{
    MessageParcel data;
    MessageParcel reply;
    int32_t result = FA_RET_ERROR;
    bool ret = SendRequest(FACE_AUTH_GET_ANGLEDIM, data, reply);
    if (ret) {
        result = reply.ReadInt32();
        FACEAUTH_LABEL_LOGI("result = %{public}d", result);
    }
    return result;
}

bool FaceAuthProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    FACEAUTH_LABEL_LOGI("enter");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        FACEAUTH_LABEL_LOGE("failed to get remote.");
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    int32_t result = remote->SendRequest(code, data, reply, option);
    if (result != OHOS::NO_ERROR) {
        FACEAUTH_LABEL_LOGE("failed to SendRequest.result = %{public}d", result);
        return false;
    }
    return true;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS