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

#include "adaptor_algorithm.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include "buffer.h"
#include "defines.h"
#include "face_auth_log_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const int OPENSSL_SUCCESS = 1;
const uint32_t ED25519_FIX_PRIKEY_BUFFER_SIZE = 32;
const uint32_t ED25519_FIX_PUBKEY_BUFFER_SIZE = 32;
const uint32_t SHA256_DIGEST_SIZE = 32;
const uint32_t SHA512_DIGEST_SIZE = 64;

static KeyPair *CreateEd25519KeyPair(void)
{
    KeyPair *keyPair = new (std::nothrow) KeyPair;
    if (keyPair == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "keyPair is nullptr");
        return nullptr;
    }
    keyPair->pubKey = CreateBuffer(ED25519_FIX_PUBKEY_BUFFER_SIZE);
    if (keyPair->pubKey == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "no memory for pub key");
        delete keyPair;
        return nullptr;
    }
    keyPair->priKey = CreateBuffer(ED25519_FIX_PRIKEY_BUFFER_SIZE);
    if (keyPair->priKey == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "no memory for pri key");
        DestoryBuffer(keyPair->pubKey);
        delete keyPair;
        return nullptr;
    }
    return keyPair;
}

void DestoryKeyPair(KeyPair *keyPair)
{
    if (keyPair == nullptr) {
        return;
    }
    if (keyPair->pubKey != nullptr) {
        DestoryBuffer(keyPair->pubKey);
    }
    if (keyPair->priKey != nullptr) {
        DestoryBuffer(keyPair->priKey);
    }
    delete keyPair;
}

bool IsEd25519KeyPairValid(const KeyPair *keyPair)
{
    if (keyPair == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "invalid key pair");
        return false;
    }
    if (!CheckBufferWithSize(keyPair->pubKey, ED25519_FIX_PUBKEY_BUFFER_SIZE)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "invalid pub key");
        return false;
    }
    if (!CheckBufferWithSize(keyPair->priKey, ED25519_FIX_PRIKEY_BUFFER_SIZE)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "invalid pri key");
        return false;
    }
    return true;
}

KeyPair *GenerateEd25519KeyPair()
{
    KeyPair *keyPair = CreateEd25519KeyPair();
    if (keyPair == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create key pair fail");
        return nullptr;
    }
    EVP_PKEY *key = nullptr;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
    if (ctx == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "new ctx fail");
        DestoryKeyPair(keyPair);
        keyPair = nullptr;
        return keyPair;
    }
    if (EVP_PKEY_keygen_init(ctx) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "init ctx fail");
        DestoryKeyPair(keyPair);
        keyPair = nullptr;
        return keyPair;
    }
    if (EVP_PKEY_keygen(ctx, &key) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "generate key fail");
        DestoryKeyPair(keyPair);
        keyPair = nullptr;
        return keyPair;
    }
    size_t pubKeySize = keyPair->pubKey->maxSize;
    if (EVP_PKEY_get_raw_public_key(key, keyPair->pubKey->buf, &pubKeySize) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get pub key fail");
        DestoryKeyPair(keyPair);
        keyPair = nullptr;
        return keyPair;
    }
    keyPair->pubKey->contentSize = pubKeySize;
    size_t priKeySize = keyPair->priKey->maxSize;
    if (EVP_PKEY_get_raw_private_key(key, keyPair->priKey->buf, &priKeySize) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get pri key fail");
        DestoryKeyPair(keyPair);
        keyPair = nullptr;
        return keyPair;
    }
    keyPair->priKey->contentSize = priKeySize;
    if (key != nullptr) {
        EVP_PKEY_free(key);
    }
    if (ctx != nullptr) {
        EVP_PKEY_CTX_free(ctx);
    }
    return keyPair;
}

int32_t Ed25519Sign(const KeyPair *keyPair, const Buffer *data, Buffer **sign)
{
    if (!IsEd25519KeyPairValid(keyPair) || !IsBufferValid(data) || sign == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "bad param");
        return RESULT_BAD_PARAM;
    }
    int32_t ret = RESULT_GENERAL_ERROR;
    EVP_PKEY *key = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr,
        keyPair->priKey->buf, keyPair->priKey->contentSize);
    if (key == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get pri key fail");
        return ret;
    }
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get ctx fail");
        EVP_PKEY_free(key);
        return ret;
    }
    if (EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, key) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "init sign fail");
        EVP_PKEY_free(key);
        EVP_MD_CTX_free(ctx);
        return ret;
    }
    *sign = CreateBuffer(ED25519_FIX_SIGN_BUFFER_SIZE);
    if (!IsBufferValid(*sign)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create buffer fail");
        EVP_PKEY_free(key);
        EVP_MD_CTX_free(ctx);
        return ret;
    }
    size_t signSize = (*sign)->maxSize;
    if (EVP_DigestSign(ctx, (*sign)->buf, &signSize, data->buf, data->contentSize) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "sign fail");
        DestoryBuffer(*sign);
        *sign = nullptr;
        EVP_PKEY_free(key);
        EVP_MD_CTX_free(ctx);
        return ret;
    }
    (*sign)->contentSize = signSize;
    ret = RESULT_SUCCESS;
    EVP_PKEY_free(key);
    EVP_MD_CTX_free(ctx);
    return ret;
}

int32_t Ed25519Verify(const Buffer *pubKey, const Buffer *data, const Buffer *sign)
{
    if (!CheckBufferWithSize(pubKey, ED25519_FIX_PUBKEY_BUFFER_SIZE) || !IsBufferValid(data) ||
        !CheckBufferWithSize(sign, ED25519_FIX_SIGN_BUFFER_SIZE)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "bad param");
        return RESULT_BAD_PARAM;
    }
    int32_t ret = RESULT_GENERAL_ERROR;
    EVP_PKEY *key = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, pubKey->buf, pubKey->contentSize);
    if (key == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get pub key fail");
        return ret;
    }
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get ctx fail");
        EVP_PKEY_free(key);
        return ret;
    }
    if (EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, key) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "init verify fail");
        EVP_PKEY_free(key);
        EVP_MD_CTX_free(ctx);
        return ret;
    }
    if (EVP_DigestVerify(ctx, sign->buf, sign->contentSize, data->buf, data->contentSize) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "verify fail");
        EVP_PKEY_free(key);
        EVP_MD_CTX_free(ctx);
        return ret;
    }
    ret = RESULT_SUCCESS;
    EVP_PKEY_free(key);
    EVP_MD_CTX_free(ctx);
    return ret;
}

static int32_t IamHmac(const EVP_MD *alg,
    const Buffer *hmacKey, const Buffer *data, Buffer *hmac)
{
    if (!IsBufferValid(hmacKey) || hmacKey->contentSize > INT_MAX ||
        !IsBufferValid(data) || !IsBufferValid(hmac) || hmac->maxSize > UINT_MAX) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "bad param");
        return RESULT_BAD_PARAM;
    }
    unsigned int hmacSize = hmac->maxSize;
    uint8_t *hmacData = HMAC(alg, hmacKey->buf, (int)hmacKey->contentSize, data->buf, data->contentSize,
        hmac->buf, &hmacSize);
    if (hmacData == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "hmac fail");
        return RESULT_GENERAL_ERROR;
    }
    hmac->contentSize = hmacSize;
    return RESULT_SUCCESS;
}

int32_t HmacSha256(const Buffer *hmacKey, const Buffer *data, Buffer **hmac)
{
    const EVP_MD *alg = EVP_sha256();
    if (alg == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "no algo");
        return RESULT_GENERAL_ERROR;
    }
    *hmac = CreateBuffer(SHA256_DIGEST_SIZE);
    if (*hmac == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create buffer fail");
        return RESULT_NO_MEMORY;
    }
    if (IamHmac(alg, hmacKey, data, *hmac) != RESULT_SUCCESS) {
        DestoryBuffer(*hmac);
        *hmac = nullptr;
        FACEAUTH_HILOGE(MODULE_SERVICE, "hmac fail");
        return RESULT_GENERAL_ERROR;
    }
    return RESULT_SUCCESS;
}

int32_t HmacSha512(const Buffer *hmacKey, const Buffer *data, Buffer **hmac)
{
    const EVP_MD *alg = EVP_sha512();
    if (alg == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "no algo");
        return RESULT_GENERAL_ERROR;
    }
    *hmac = CreateBuffer(SHA512_DIGEST_SIZE);
    if (*hmac == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create buffer fail");
        return RESULT_NO_MEMORY;
    }
    if (IamHmac(alg, hmacKey, data, *hmac) != RESULT_SUCCESS) {
        DestoryBuffer(*hmac);
        *hmac = nullptr;
        FACEAUTH_HILOGE(MODULE_SERVICE, "hmac fail");
        return RESULT_GENERAL_ERROR;
    }
    return RESULT_SUCCESS;
}

int32_t SecureRandom(uint8_t *buffer, uint32_t size)
{
    if (buffer == nullptr || size > INT_MAX) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "bad param");
        return RESULT_BAD_PARAM;
    }
    if (RAND_bytes(buffer, (int)size) != OPENSSL_SUCCESS) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "rand fail");
        return RESULT_GENERAL_ERROR;
    }
    return RESULT_SUCCESS;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
