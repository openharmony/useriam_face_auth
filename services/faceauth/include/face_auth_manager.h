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
#ifndef FACE_AUTH_MANAGER_H
#define FACE_AUTH_MANAGER_H
#include "face_auth_query_callback.h"
#include "face_auth_executor_callback.h"
#include "face_auth_event.h"
#include "face_auth_ca.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
class FaceAuthSequence;
class FaceAuthManager {
public:
    static std::shared_ptr<FaceAuthManager> GetInstance();
    ~FaceAuthManager()=default;
    FaceAuthManager()=default;
    int32_t Init();
    int32_t Release();
    void RegisterExecutor();
    // about authenticate
    int32_t Authenticate(const AuthParam &param);
    void HandleCallAuthenticate(const AuthParam &param);
    int32_t CancelAuth(const AuthParam &param);
    // about enroll
    int32_t Enrollment(const EnrollParam &param);
    void HandleCallEnroll(const EnrollParam &param);
    int32_t CancelEnrollment(const EnrollParam &param);
    // about remove
    int32_t Remove(const RemoveParam &param);
    void HandleCallRemove(const RemoveParam &param);
    // about algorithm
    FIRetCode InitAlgorithm(std::string bundleName);
    FIRetCode ReleaseAlgorithm(std::string bundleName);
    // about messager
    void SetExecutorMessenger(const sptr<AuthResPool::IExecutorMessenger> &messager);
private:
    static std::shared_ptr<FaceAuthManager> manager_;
    static std::mutex mutex_;
    static sptr<AuthResPool::IExecutorMessenger> executorMessenger_;
    static std::shared_ptr<FaceAuthEventHandler> handler_;
    static std::shared_ptr<AppExecFwk::EventRunner> runner_;
    static std::shared_ptr<AuthResPool::QueryCallback> queryCallback_;
    static std::shared_ptr<FaceAuthExecutorCallback> executorCallback_;
    std::map<std::string, int32_t> bundleNameList_;
private:
    FaceAuthManager(const FaceAuthManager&)=delete;
    FaceAuthManager& operator=(const FaceAuthManager&)=delete;
    void QueryRegStatus();
    bool IsAlgorithmInited();
    AlgoResult IsNeedAlgoLoad(std::string bundleName);
    AlgoResult IsNeedAlgoRelease(std::string bundleName);
    uint32_t GenerateEventId();
    void SendData(uint64_t scheduleId,
                         uint64_t transNum,
                         int32_t srcType,
                         int32_t dstType,
                         pAuthMessage msg);
    void Finish(uint64_t scheduleId,
                       int32_t srcType,
                       int32_t resultCode,
                       pAuthAttributes finalResult);
    FIRetCode OperForAlgorithm(uint64_t scheduleID);
    void HandleAlgoResult(uint64_t scheduleID);
    int32_t OpenCamera();
    int32_t GetAuthMessage(uint64_t scheduleID);
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // FACE_AUTH_MANAGER_H
