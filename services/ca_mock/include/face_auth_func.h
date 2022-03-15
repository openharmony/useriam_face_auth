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

#ifndef FACE_AUTHTA_FUNC_H
#define FACE_AUTHTA_FUNC_H

#include <vector>
#include "defines.h"
#include "buffer.h"
#include "coauth_info_define.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
#define TAG_AND_LEN_BYTE 8
#define TAG_ANG_LEN_T 12
#define TAG_AND_LEN_S 16
#define MAX_TLV_LEN 200
#define SIGN_DATA_LEN 64
#define PIN_RET_TYPE_LEN 8
#define PIN_RET_DATA_LEN 72
#define FACE_AUTH_CAPABILITY_LEVEL 2
#define ED25519_FIX_PUBKEY_BUFFER_SIZE 32
#define ED25519_FIX_PRIKEY_BUFFER_SIZE 64
#define FACE_EXECUTOR_SECURITY_LEVEL 1
#define PIN_AUTH_ABILITY 7
#define FACE_AUTH_ABILITY 7
#define RESULT_TLV_LEN 160U
#define CONST_PUB_KEY_LEN 32U

ResultCode GenerateRetTlv(uint32_t result, uint64_t scheduleId, uint64_t subType, uint64_t templatedId, Buffer *retTlv);
ResultCode GenerateKeyPair();
ResultCode SetResultTlv(Buffer *retTlv, std::vector<uint8_t> &resultTlv);
ResultCode DoGetExecutorInfo(std::vector<uint8_t> &vPubKey, uint32_t &esl, uint64_t &authAbility);
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // FACE_AUTHTA_FUNC_H
