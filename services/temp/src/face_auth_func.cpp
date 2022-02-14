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

#include "face_auth_func.h"
#include "adaptor_algorithm.h"
#include "buffer.h"
#include "securec.h"
#include "face_auth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
static KeyPair *g_keyPair = nullptr;

ResultCode GenerateKeyPair()
{
    DestoryKeyPair(g_keyPair);
    g_keyPair = GenerateEd25519KeyPair();
    if (g_keyPair == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GenerateEd25519Keypair fail!");
        return RESULT_GENERAL_ERROR;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "GenerateKeyPair success");
    return RESULT_SUCCESS;
}

static ResultCode WriteTlvHead(const AuthAttributeType type, const uint32_t length, Buffer *buf)
{
    int32_t tempType = type;
    if (memcpy_s(buf->buf + buf->contentSize, buf->maxSize - buf->contentSize, &tempType, sizeof(tempType)) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "copy type fail.");
        return RESULT_BAD_COPY;
    }
    buf->contentSize += sizeof(tempType);
    if (memcpy_s(buf->buf + buf->contentSize, buf->maxSize - buf->contentSize, &length, sizeof(length)) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "copy length fail.");
        return RESULT_BAD_COPY;
    }
    buf->contentSize += sizeof(length);
    return RESULT_SUCCESS;
}

static ResultCode WriteTlv(const AuthAttributeType type, const uint32_t length, const uint8_t *value, Buffer *buf)
{
    if (WriteTlvHead(type, length, buf) != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "copy head fail.");
        return RESULT_BAD_COPY;
    }
    if (memcpy_s(buf->buf + buf->contentSize, buf->maxSize - buf->contentSize, value, length) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "copy value fail.");
        return RESULT_BAD_COPY;
    }
    buf->contentSize += length;
    return RESULT_SUCCESS;
}

static Buffer *GetDataTlvContent(uint32_t result, uint64_t scheduleId, uint64_t subType, uint64_t templatedId)
{
    Buffer *ret = CreateBuffer(MAX_TLV_LEN);
    if (!IsBufferValid(ret)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "no memory.");
        return nullptr;
    }
    uint32_t acl = FACE_AUTH_CAPABILITY_LEVEL;
    if (WriteTlv(AUTH_RESULT_CODE, sizeof(result), (const uint8_t *)&result, ret) != RESULT_SUCCESS ||
        WriteTlv(AUTH_TEMPLATE_ID, sizeof(templatedId), (const uint8_t *)&templatedId, ret) != RESULT_SUCCESS ||
        WriteTlv(AUTH_SESSION_ID, sizeof(scheduleId), (const uint8_t *)&scheduleId, ret) != RESULT_SUCCESS ||
        WriteTlv(AUTH_SUBTYPE, sizeof(subType), (const uint8_t *)&subType, ret) != RESULT_SUCCESS ||
        WriteTlv(AUTH_CAPABILITY_LEVEL, sizeof(acl), (const uint8_t *)&acl, ret) != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "write tlv fail.");
        DestoryBuffer(ret);
        return nullptr;
    }
    return ret;
}

ResultCode GenerateRetTlv(uint32_t result, uint64_t scheduleId, uint64_t subType, uint64_t templatedId,
    Buffer *retTlv)
{
    if (!IsBufferValid(retTlv) || !IsEd25519KeyPairValid(g_keyPair)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "param is invalid.");
        return RESULT_BAD_PARAM;
    }
    Buffer *dataContent = GetDataTlvContent(result, scheduleId, subType, templatedId);
    if (!IsBufferValid(dataContent)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get data content fail.");
        return RESULT_BAD_COPY;
    }
    Buffer *signContent = nullptr;
    if (Ed25519Sign(g_keyPair, dataContent, &signContent) != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "sign data fail.");
        DestoryBuffer(dataContent);
        return RESULT_GENERAL_ERROR;
    }
    uint32_t rootLen = TAG_AND_LEN_BYTE + dataContent->contentSize + TAG_AND_LEN_BYTE + ED25519_FIX_SIGN_BUFFER_SIZE;
    if (WriteTlvHead(AUTH_ROOT, rootLen, retTlv) != RESULT_SUCCESS ||
        WriteTlv(AUTH_EXECUTOR_DATA, dataContent->contentSize, dataContent->buf, retTlv) != RESULT_SUCCESS ||
        WriteTlv(AUTH_SIGNATURE, signContent->contentSize, signContent->buf, retTlv) != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "write tlv fail.");
        DestoryBuffer(dataContent);
        DestoryBuffer(signContent);
        return RESULT_BAD_COPY;
    }
    DestoryBuffer(dataContent);
    DestoryBuffer(signContent);
    return RESULT_SUCCESS;
}
ResultCode SetResultTlv(Buffer *retTlv, std::vector<uint8_t> &resultTlv)
{
    resultTlv.resize(retTlv->contentSize);
    if (memcpy_s(&resultTlv[0], retTlv->contentSize, retTlv->buf, retTlv->contentSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "copy retTlv to resultTlv fail!");
        return RESULT_GENERAL_ERROR;
    }
    return RESULT_SUCCESS;
}
ResultCode DoGetExecutorInfo(std::vector<uint8_t> &vPubKey, uint32_t &esl, uint64_t &authAbility)
{
    if (!IsEd25519KeyPairValid(g_keyPair)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "key pair not init!");
        return RESULT_NEED_INIT;
    }
    uint32_t pubKeyLen = CONST_PUB_KEY_LEN;
    uint8_t pubKey[CONST_PUB_KEY_LEN];
    if (GetBufferData(g_keyPair->pubKey, pubKey, &pubKeyLen) != RESULT_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetBufferData fail!");
        return RESULT_UNKNOWN;
    }
    int index = 0;
    vPubKey.clear();
    for (index = 0; index < CONST_PUB_KEY_LEN; index++) {
        vPubKey.push_back(pubKey[index]);
    }
    esl = FACE_EXECUTOR_SECURITY_LEVEL;
    authAbility = FACE_AUTH_AIBNILITY;
    return RESULT_SUCCESS;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
