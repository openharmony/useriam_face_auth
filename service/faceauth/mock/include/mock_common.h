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

#ifndef FACEAUTH_SERVICES_INCLUDE_MOCK_COMMON_H
#define FACEAUTH_SERVICES_INCLUDE_MOCK_COMMON_H

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace {
static const char *CONFIG_FILENAME("/data/config.dat");
static const char *FACEID_FILENAME("/data/faceId.dat");
static const char *INIT_FILENAME("/data/init.dat");
static const char *RELEASE_FILENAME("/data/release.dat");
static const char *POST_FILENAME("/data/post.dat");
static const char *PRE_FILENAME("/data/pre.dat");
static const int32_t AHGLE_DIM = 3;
static const int32_t CASE_NUM = 100;
static const int32_t CODE_NUM = 5;
static const int32_t RESULT_CODE = 6;
static const int32_t IFACE_OVER_MAX_FACES = 901;
static const int32_t IFACE_ENROLL_HAS_REGISTERED = 902;
static const int32_t IFACE_CAMERA_FAIL = 903;
static const int32_t IFACE_TIME_OUT = 904;
static const int32_t IFACE_FACE_DETECTED = 38;
static const std::string TEST_ANGLE = "999";
static const std::string TEST_ENROLL_SUCCESS = "998";
static const std::string TEST_OVER_MAX_FACES = "901";
static const std::string TEST_ENROLL_HAS_REGISTERED = "902";
static const std::string TEST_CAMERA_FAIL = "903";
static const std::string TEST_ANGLE_TIMEOUT = "905";
static const int32_t DELAY_TWO_SECOND = 222;
static const int32_t FACE_AUTH_GETRESULT_FAIL = 1001;
static const int32_t FACE_AUTH_GETRESULT_TIMEOUT = 1002;
static const int32_t FACE_AUTH_FAIL = 1003;
static const int32_t FACE_AUTH_INIT_TIMEOUT = 1004;
static const std::string FACE_AUTH_GETRESULT_FAIL_STRING = "1001";
static const std::string FACE_AUTH_GETRESULT_TIMEOUT_STRING = "1002";
static const std::string FACE_AUTH_FAIL_STRING = "1003";
static const std::string FACE_AUTH_INIT_TIMEOUT_STRING = "1004";
} // namespace
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

#endif // FACEAUTH_SERVICES_INCLUDE_MOCK_COMMON_H
