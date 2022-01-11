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

#include "face_auth_ca.h"
#include <cmath>
#include <fstream>
#include <string>
#include <securec.h>
#include "faceauth_log_wrapper.h"
#include "face_auth_algo_impl.h"
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::mutex FaceAuthCa::mutex_;
std::shared_ptr<FaceAuthCa> FaceAuthCa::instance_ = nullptr;
static const char *TEMP_FILENAME("/data/temp.dat");
static const int32_t STABILITY_FACE_ID = 99;
std::shared_ptr<FaceAuthCa> FaceAuthCa::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<FaceAuthCa>();
        }
    }
    return instance_;
}

FaceAuthCa::FaceAuthCa() : challenge_(0), isInitFail_(false)
{}

FaceAuthCa::~FaceAuthCa()
{}

void FaceAuthCa::ReadFile()
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa ReadFile start");
    std::ifstream mystream(CONFIG_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_LABEL_LOGI("FaceAuthCa ReadFile fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_LABEL_LOGI("casenum is %{public}s", s.c_str());
    CheckPostFile(s);
    mystream.close();
    return;
}

void FaceAuthCa::CheckFile(std::string s)
{
    if (s.compare(FACE_AUTH_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_INIT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    return;
}

void FaceAuthCa::ReadPostFile()
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa ReadFile start");
    std::ifstream mystream(POST_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_LABEL_LOGI("FaceAuthCa ReadFile fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_LABEL_LOGI("casenum is %{public}s", s.c_str());
    CheckPostFile(s);
    mystream.close();
    return;
}

void FaceAuthCa::ReadPreFile()
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa ReadFile start");
    std::ifstream mystream(PRE_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_LABEL_LOGI("FaceAuthCa ReadFile fail");
        return;
    }
    char casenum[CASE_NUM];
    mystream.getline(casenum, CASE_NUM);
    std::string s = casenum;
    FACEAUTH_LABEL_LOGI("casenum is %{public}s", s.c_str());
    CheckPostFile(s);
    mystream.close();
    return;
}

void FaceAuthCa::CheckPostFile(std::string s)
{
    if (s.compare(FACE_AUTH_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_INIT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    return;
}

void FaceAuthCa::CheckPreFile(std::string s)
{
    if (s.compare(FACE_AUTH_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_INIT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_TIMEOUT_STRING) == 0) {
        isInitFail_ = true;
    }
    if (s.compare(FACE_AUTH_GETRESULT_FAIL_STRING) == 0) {
        isInitFail_ = true;
    }
    return;
}

int32_t FaceAuthCa::Prepare(std::unique_ptr<PrepareParam> param)
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa Prepare");
    return 0;
}

int32_t FaceAuthCa::PreEnroll(int64_t &challenge, int32_t type)
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa PreEnroll");
    ReadPreFile();
    FACEAUTH_LABEL_LOGI("FaceAuthCa PreEnroll isInitFail_ is %{public}d", isInitFail_);
    if (isInitFail_) {
        FACEAUTH_LABEL_LOGI("FaceAuthCa PreEnroll isInitFail_ is %{public}d", isInitFail_);
        remove(PRE_FILENAME);
        isInitFail_ = false;
        return -1;
    }
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &t);
    int64_t elapsedTime {((t.tv_sec) * SEC_TO_NANOSEC + t.tv_nsec)};
    size_t elapsedHash = std::hash<std::string>()(std::to_string(elapsedTime));
    int64_t rand = static_cast<int64_t>(elapsedHash);
    if (rand < 0) {
        rand = abs(rand);
    }
    challenge = rand;
    return 0;
}

int32_t FaceAuthCa::PostEnroll(int32_t type)
{
    FACEAUTH_LABEL_LOGE("FaceAuthCa PostEnroll");
    ReadPostFile();
    FACEAUTH_LABEL_LOGI("FaceAuthCa PostEnroll isInitFail_ is %{public}d", isInitFail_);
    if (isInitFail_) {
        FACEAUTH_LABEL_LOGI("FaceAuthCa PostEnroll isInitFail_ is %{public}d", isInitFail_);
        remove(POST_FILENAME);
        isInitFail_ = false;
        return -1;
    }
    challenge_ = 0;
    return 0;
}

int32_t FaceAuthCa::DeleteUser(uint32_t uid)
{
    return 0;
}

int32_t FaceAuthCa::SetActiveGroup(const uint32_t uid, std::list<int32_t> &faceInfolist)
{
    std::ifstream tempstream(TEMP_FILENAME);
    if (tempstream.is_open()) {
        faceInfolist.push_back(atoi("1"));
        remove(TEMP_FILENAME);
        return 0;
    }
    std::ifstream mystream(FACEID_FILENAME);
    if (!mystream.is_open()) {
        FACEAUTH_LABEL_LOGI("FaceAuthCa SetActiveGroup openfile fail");
        EnrollList_.clear();
        return 0;
    } else {
        FACEAUTH_LABEL_LOGI("FaceAuthCa SetActiveGroup openfile success");
        char buffer[CODE_NUM];
        while (mystream.getline(buffer, CODE_NUM)) {
            mystream >> buffer;
            FACEAUTH_LABEL_LOGI("FaceAuthCa.challenge str is %{public}d", atoi(buffer));
            faceInfolist.push_back(atoi(buffer));
            EnrollList_.clear();
            EnrollList_.push_back(atoi(buffer));
        }
    }
    mystream.close();
    FACEAUTH_LABEL_LOGI("FaceAuthCa SetActiveGroup faceInfolist.size is %{public}d", faceInfolist.size());
    if (uid != 0) {
        faceInfolist.clear();
    }

    return 0;
}

int32_t FaceAuthCa::VerifyAuthToken(std::unique_ptr<uint8_t[]> authToken, uint32_t length, int32_t type)
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa VerifyAuthToken");
    std::string str = (char *)authToken.get();
    FACEAUTH_LABEL_LOGI("VerifyAuthToken: str = %{public}s", str.c_str());
    return 0;
}

int32_t FaceAuthCa::GetAuthToken(std::unique_ptr<uint8_t[]> &authToken, uint32_t &len)
{
    if (challenge_ != 0) {
        uint8_t at[] = {'A', 'u', 't', 'h', 'T', 'o', 'k', 'e', 'n', '\0'};
        if (memcpy_s(authToken.get(), TOKEN_NUM, at, sizeof(at)) != EOK) {
            FACEAUTH_LABEL_LOGE("GetAuthToken memcpy_s fail");
            return -1;
        }
        len = sizeof(at);
    }
    return 0;
}

int32_t FaceAuthCa::SetChallenge(int64_t challenge)
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa.challenge is %{private}lld", challenge);
    if (challenge < 0) {
        FACEAUTH_LABEL_LOGE("Parameter check error.challenge is %{private}lld", challenge);
        return -1;
    }
    challenge_ = challenge;
    return 0;
}

int32_t FaceAuthCa::DeleteTemolate(int32_t faceId)
{
    FACEAUTH_LABEL_LOGI("FaceAuthCa DeleteTemplate");
    if (faceId == STABILITY_FACE_ID) {
        FACEAUTH_LABEL_LOGI("Stability Test nothing to do!");
        return -1;
    } else if (faceId == 0) {
        FACEAUTH_LABEL_LOGI("faceId remove all");
        std::ofstream file;
        file.open(FACEID_FILENAME, std::ios_base::out | std::ios_base::trunc);
        file << " ";
        file.close();
        remove(FACEID_FILENAME);
        EnrollList_.clear();
    } else {
        auto iter = std::find(EnrollList_.begin(), EnrollList_.end(), faceId);
        if (iter == EnrollList_.end()) {
            return -1;
        }
        EnrollList_.remove(faceId);
        std::ofstream file;
        file.open(FACEID_FILENAME, std::ios_base::out | std::ios_base::trunc);
        if (!file.is_open()) {
            return -1;
        }
        FACEAUTH_LABEL_LOGE("FaceAuthCa DeleteTemolate open success");
        std::list<int32_t>::iterator iters;
        for (iters = EnrollList_.begin(); iters != EnrollList_.end(); ++iters) {
            file << std::to_string(*iters);
            file << " ";
        }
        file.close();
    }
    return 0;
}

int32_t FaceAuthCa::DeleteTemplateStep2()
{
    FACEAUTH_LABEL_LOGE("FaceAuthCa DeleteTemplateStep2");
    ReadFile();
    FACEAUTH_LABEL_LOGI("FaceAuthCa DeleteTemplateStep2 isInitFail_ is %{public}d", isInitFail_);
    if (isInitFail_) {
        FACEAUTH_LABEL_LOGI("FaceAuthCa DeleteTemplateStep2 isInitFail_ is %{public}d", isInitFail_);
        isInitFail_ = false;
        remove(CONFIG_FILENAME);
        return -1;
    }
    return 0;
}
}  // namespace FaceAuth
}  // namespace UserIAM
}  // namespace OHOS