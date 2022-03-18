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

#include "face_auth_manager.h"
#include <openssl/bn.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <iservice_registry.h>
#include "securec.h"
#include "face_auth_log_wrapper.h"
#include "face_auth_event_handler.h"
#include "auth_executor_registry.h"
#include "useridm_client.h"
#include "useridm_info.h"
#include "face_auth_get_info_callback.h"
#include "auth_message.h"
#include "coauth_info_define.h"
#include "face_auth_thread_pool.h"
#include "face_auth_req.h"
#include "face_auth_camera.h"
#include "face_auth_defines.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const int RAND_NUM_BITS = 32;
const int TOP = -1;
const int BOTTOM = 0;
const int INVALID_EVENT_ID = -1;
static const std::string FACE_LOCAL_INIT_ALGO_NAME = "face_auth_local_init";
std::shared_ptr<FaceAuthManager> FaceAuthManager::manager_ = nullptr;
std::mutex FaceAuthManager::mutex_;
sptr<AuthResPool::IExecutorMessenger> FaceAuthManager::executorMessenger_;
std::shared_ptr<FaceAuthEventHandler> FaceAuthManager::handler_ = nullptr;
std::shared_ptr<AppExecFwk::EventRunner> FaceAuthManager::runner_ = nullptr;
std::shared_ptr<AuthResPool::QueryCallback> FaceAuthManager::queryCallback_ = nullptr;
std::shared_ptr<FaceAuthExecutorCallback> FaceAuthManager::executorCallback_ = nullptr;
static void CheckSystemAbility();

std::shared_ptr<FaceAuthManager> FaceAuthManager::GetInstance()
{
    if (manager_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (manager_ == nullptr) {
            manager_ = std::make_shared<FaceAuthManager>();
        }
    }
    return manager_;
}

int32_t FaceAuthManager::Init()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    // run event handler
    std::string threadName("FaceAuthEventRunner");
    runner_ = AppExecFwk::EventRunner::Create(threadName);
    if (runner_ == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "failed to create a runner.");
        return FA_RET_ERROR;
    }
    runner_->Run();
    handler_ = std::make_shared<FaceAuthEventHandler>(runner_);
    // start CA
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create FaceAuthCA instance failed.");
        return FA_RET_ERROR;
    }
    if (FA_RET_OK != faceAuthCA->Init()) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Init CA failed.");
        return FA_RET_ERROR;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthManager::Init Success.");
    // check register state
    std::thread checkThread(OHOS::UserIAM::FaceAuth::CheckSystemAbility);
    checkThread.join();
    QueryRegStatus();
    return FA_RET_OK;
}

void CheckSystemAbility()
{
    const int CHECK_TIMES = 3;
    const int SLEEP_TIME = 1;
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Failed to get system ability manager");
        return;
    }
    for (int i = 0; i < CHECK_TIMES; i++) {
        bool isExist = false;
        sam->CheckSystemAbility(SUBSYS_USERIAM_SYS_ABILITY_AUTHEXECUTORMGR, isExist);
        if (!isExist) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "AUTHEXECUTORMGR is not exist, start ability failed, to do next");
        } else {
            FACEAUTH_HILOGI(MODULE_SERVICE, "AUTHEXECUTORMGR is exist, start AUTHEXECUTORMGR ability success");
            return;
        }
        if (i < CHECK_TIMES - 1) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "begin sleep");
            sleep(SLEEP_TIME);
            FACEAUTH_HILOGI(MODULE_SERVICE, "end sleep");
        }
    }
    FACEAUTH_HILOGE(MODULE_SERVICE, "start AUTHEXECUTORMGR ability all failed");
}

void FaceAuthManager::QueryRegStatus()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    // get executor info
    std::shared_ptr<AuthResPool::AuthExecutor> executorInfo = std::make_shared<AuthResPool::AuthExecutor>();
    std::vector<uint8_t> pubKey;
    uint32_t esl = 0;
    uint64_t authAbility = 0;
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create FaceAuthCA instance failed.");
        return;
    }
    int32_t ret = faceAuthCA->GetExecutorInfo(pubKey, esl, authAbility);
    if (FA_RET_OK != ret) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetExecutorInfo failed.");
        return;
    }
    // set executor info
    executorInfo->SetAuthType(FACE);
    executorInfo->SetExecutorType(TYPE_ALL_IN_ONE);

    if (queryCallback_== nullptr) {
        queryCallback_ = std::make_shared<FaceAuthQueryCallback>();
    }
    AuthResPool::AuthExecutorRegistry::GetInstance().QueryStatus(*executorInfo, queryCallback_);
}

void FaceAuthManager::RegisterExecutor()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    // get executor info
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "create FaceAuthCA instance failed.");
        return;
    }
    std::vector<uint8_t> pubKey;
    uint32_t esl = 0;
    uint64_t authAbility = 0;
    int32_t ret = faceAuthCA->GetExecutorInfo(pubKey, esl, authAbility);
    if (FA_RET_OK != ret) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetExecutorInfo failed.");
        return;
    }
    std::shared_ptr<AuthResPool::AuthExecutor> executorInfo = std::make_shared<AuthResPool::AuthExecutor>();
    // set executor info
    executorInfo->SetPublicKey(pubKey);
    executorInfo->SetExecutorSecLevel(static_cast<ExecutorSecureLevel>(esl));
    executorInfo->SetAuthAbility(authAbility);
    executorInfo->SetAuthType(FACE);
    executorInfo->SetExecutorType(TYPE_ALL_IN_ONE);
    executorCallback_ = std::make_shared<FaceAuthExecutorCallback>();
    // executorCallback_ release
    uint64_t regRet = AuthResPool::AuthExecutorRegistry::GetInstance().Register(executorInfo, executorCallback_);
    if (regRet != 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthInitSeq::RegisterExecutor successful.executor id = %{public}" PRIu64,
            regRet);
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthInitSeq::RegisterExecutor failed.");
    }
    return;
}

void FaceAuthManager::VerifyAuthInfo()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    const int32_t ALL_INFO_GET_USER_ID = -1;

    std::shared_ptr<FaceAuthGetInfoCallback> getInfoCallback = std::make_shared<FaceAuthGetInfoCallback>();
    int32_t ret = UserIDM::UserIDMClient::GetInstance().GetAuthInfo(ALL_INFO_GET_USER_ID, UserIDM::AuthType::FACE,
        getInfoCallback);
    if (ret == 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s get auth info success.", __PRETTY_FUNCTION__);
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "%{public}s get auth info failed.", __PRETTY_FUNCTION__);
    }
    return;
}

int32_t FaceAuthManager::Release()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA != nullptr) {
        faceAuthCA->Close();
    }
    if (runner_ != nullptr) {
        runner_.reset();
    }
    return FA_RET_OK;
}

ResultCodeForCoAuth FaceAuthManager::AddAuthenticationRequest(const AuthParam &param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoAuth]scheduleID = %{public}" PRIu64 ".", param.scheduleID);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoAuth]templateID = %{public}s.", getMaskedString(param.templateID).c_str());
    // check req info list is full
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_LOCAL_AUTH)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Auth is Max.");
        return ResultCodeForCoAuth::BUSY;
    }
    // send event
    FaceInfo faceInfo = {};
    faceInfo.eventId = GenerateEventId();
    if (faceInfo.eventId == INVALID_EVENT_ID) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceInfo.eventId is invalid.");
        return ResultCodeForCoAuth::GENERAL_ERROR;
    }
    faceInfo.uId = param.callerUID;
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::LOW;
    auto authInfo = std::make_unique<AuthParam>(param);
    handler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return ResultCodeForCoAuth::SUCCESS;
}

void FaceAuthManager::DoAuthenticate(const AuthParam &param)
{
    this->InitAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
    // open camera and send image to algorithm
    if (OpenCamera(nullptr) != FA_RET_OK) {
        // RK3568 no support camera, temporary ignore error
        FACEAUTH_HILOGE(MODULE_SERVICE, "Ignore open camera fail.");
    }
    // start algorithm operation
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA instance is null");
        return;
    }
    AlgorithmOperation algorithmOperation = Auth;
    AlgorithmParam algorithmParam;
    algorithmParam.scheduleId = param.scheduleID;
    algorithmParam.templateId = param.templateID;
    int32_t ret = faceAuthCA->StartAlgorithmOperation(algorithmOperation, algorithmParam);
    if (ret != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "StartAlgorithmOperation failed");
        return;
    }
    // wait authentication process done
    ret = WaitAlgorithmProcessDone(param.scheduleID);
    if (ret != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "authentication process result is %{public}d.", ret);
        return;
    }
   // close camera
    std::shared_ptr<FaceAuthCamera> faceAuthCamera = FaceAuthCamera::GetInstance();
    if (faceAuthCamera == nullptr) {
        return;
    }
    faceAuthCamera->CloseCamera();
    this->ReleaseAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
    // remove require info
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
}

ResultCodeForCoAuth FaceAuthManager::AddEnrollmentRequest(const EnrollParam &param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoEnroll]scheduleID = %{public}" PRIu64 ".", param.scheduleID);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoEnroll]templateID = %{public}s.", getMaskedString(param.templateID).c_str());
    // check req info list is full
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_ENROLL)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Enroll is Max.");
        return ResultCodeForCoAuth::BUSY;
    }
    // send event
    FaceInfo faceInfo = {};
    faceInfo.eventId = GenerateEventId();
    if (faceInfo.eventId == INVALID_EVENT_ID) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceInfo.eventId is invalid.");
        return ResultCodeForCoAuth::GENERAL_ERROR;
    }
    faceInfo.uId = param.callerUID;
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::HIGH;
    auto authInfo = std::make_unique<EnrollParam>(param);
    handler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return ResultCodeForCoAuth::SUCCESS;
}

void FaceAuthManager::DoEnroll(const EnrollParam &param)
{
    this->InitAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
    // open camera and send image to algorithm
    if (OpenCamera(param.producer) != FA_RET_OK) {
        // RK3568 no support camera, temporary ignore error
        FACEAUTH_HILOGI(MODULE_SERVICE, "Ignore open camera fail.");
    }
    // check algorithm init
    if (IsAlgorithmInited()) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthManager::Enroll Need Init.");
        return;
    }
    // start algorithm operation
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA instance is null");
        return;
    }
    AlgorithmOperation algorithmOperation = Enroll;
    AlgorithmParam algorithmParam;
    algorithmParam.scheduleId = param.scheduleID;
    algorithmParam.templateId = param.templateID;
    int32_t ret = faceAuthCA->StartAlgorithmOperation(algorithmOperation, algorithmParam);
    if (ret != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "StartAlgorithmOperation failed");
        return;
    }
    // wait enroll process done
    ret = WaitAlgorithmProcessDone(param.scheduleID);
    if (ret != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "authentication process result is %{public}d.", ret);
        return;
    }
    // close camera
    std::shared_ptr<FaceAuthCamera> faceAuthCamera = FaceAuthCamera::GetInstance();
    if (faceAuthCamera == nullptr) {
        return;
    }
    faceAuthCamera->CloseCamera();
    this->ReleaseAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
    // remove require info
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
}

int32_t FaceAuthManager::AddRemoveRequest(const RemoveParam &param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoRemove]scheduleID = %{public}" PRIu64 ".", param.scheduleID);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoRemove]templateID = %{public}s.", getMaskedString(param.templateID).c_str());
    // check req info list is full
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_DEL;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_DEL)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Remove is Max.");
        return FA_RET_ERROR;
    }
    // send event
    FaceInfo faceInfo = {};
    faceInfo.eventId = GenerateEventId();
    if (faceInfo.eventId == INVALID_EVENT_ID) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceInfo.eventId is invalid.");
        return FA_RET_ERROR;
    }
    faceInfo.uId = param.callerUID;
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::IMMEDIATE;
    auto authInfo = std::make_unique<RemoveParam>(param);
    handler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return FA_RET_OK;
}

void FaceAuthManager::DoRemove(const RemoveParam &param)
{
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA instance is null.");
        return;
    }
    int32_t ret = faceAuthCA->DeleteTemplate(param.templateID);
    if (FA_RET_OK == ret) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Remove success.");
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Remove failed.");
    }
    // remove require info
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_DEL;
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
}

FIRetCode FaceAuthManager::OperForAlgorithm(uint64_t scheduleID)
{
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA is null.");
        return FI_RC_INVALID_ARGUMENT;
    }
    int32_t retCode = 0;
    std::vector<uint8_t> msgBuffer;
    while (true) {
        faceAuthCA->GetAlgorithmState(retCode, msgBuffer);
        FACEAUTH_HILOGI(MODULE_SERVICE, "get algorithm start code %{public}d, msg length %{public}d.",
            retCode, msgBuffer.size());
        if (retCode == FACE_ALGORITHM_OPERATION_BREAK) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "FACE_ALGORITHM_OPERATION_BREAK.");
            break;
        }
        if (msgBuffer.size() > 0) {
            std::shared_ptr<AuthResPool::AuthMessage> msg = std::make_shared<AuthResPool::AuthMessage>(msgBuffer);
            SendData(scheduleID, 0, TYPE_ALL_IN_ONE, TYPE_CO_AUTH, msg);
        }
    }
    HandleAlgoResult(scheduleID);
    return FI_RC_OK;
}

void FaceAuthManager::HandleAlgoResult(uint64_t scheduleID)
{
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "faceAuthCA is null.");
        return;
    }
    AlgorithmResult retResult;
    faceAuthCA->FinishAlgorithmOperation(retResult);
    FACEAUTH_HILOGI(MODULE_SERVICE, "Face auth result = %{public}d.", retResult.result);
    // return result
    pAuthAttributes authAttributes = std::make_shared<AuthResPool::AuthAttributes>();
    authAttributes->SetUint32Value(AUTH_RESULT_CODE, 0);
    authAttributes->SetUint8ArrayValue(AUTH_RESULT, retResult.coauthMsg);
    Finish(scheduleID, TYPE_ALL_IN_ONE, retResult.result, authAttributes);
}

int32_t FaceAuthManager::CancelAuth(const AuthParam &param)
{
    int32_t result = FA_RET_OK;
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    int32_t uId = param.callerUID;
    bool isSuccess = FaceAuthReq::GetInstance()->SetCancelFlagSuccess(reqType, uId);
    if (!isSuccess) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "CancelAuth failed, reqId: %{public}s,",
            getMaskedString(reqType.reqId).c_str());
        result = FA_RET_ERROR;
    } else {
        std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
        if (faceAuthCA == nullptr) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthCA instance is null");
            return result;
        }
        result = faceAuthCA->CancelAlgorithmOperation();
        if (result == FA_RET_OK) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "CancelAlgorithmOperation success");
        } else {
            FACEAUTH_HILOGE(MODULE_SERVICE, "CancelAlgorithmOperation failed");
        }
    }
    return result;
}

int32_t FaceAuthManager::CancelEnrollment(const EnrollParam &param)
{
    int32_t result = FA_RET_OK;
    FaceReqType reqType = {};
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    int32_t uId = param.callerUID;
    bool isSuccess = FaceAuthReq::GetInstance()->SetCancelFlagSuccess(reqType, uId);
    if (!isSuccess) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "CancelEnrollment failed, reqId: %{public}s",
            getMaskedString(reqType.reqId).c_str());
        result = FA_RET_ERROR;
    } else {
        std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
        if (faceAuthCA == nullptr) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthCA instance is null");
            return result;
        }
        result = faceAuthCA->CancelAlgorithmOperation();
        if (result == FA_RET_OK) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "CancelAlgorithmOperation success");
        } else {
            FACEAUTH_HILOGE(MODULE_SERVICE, "CancelAlgorithmOperation failed");
        }
    }
    return result;
}

void FaceAuthManager::SetExecutorMessenger(const sptr<AuthResPool::IExecutorMessenger> &messager)
{
    executorMessenger_ = messager;
}
void FaceAuthManager::SendData(uint64_t scheduleId, uint64_t transNum, int32_t srcType, int32_t dstType,
    pAuthMessage msg)
{
    if (executorMessenger_ != nullptr) {
        executorMessenger_->SendData(scheduleId, transNum, srcType, dstType, msg);
    } else {
        FACEAUTH_HILOGI(MODULE_SERVICE, "executorMessenger_ is null.");
    }
}

void FaceAuthManager::Finish(uint64_t scheduleId, int32_t srcType, int32_t resultCode, pAuthAttributes finalResult)
{
    if (executorMessenger_ != nullptr) {
        executorMessenger_->Finish(scheduleId, srcType, resultCode, finalResult);
    } else {
        FACEAUTH_HILOGI(MODULE_SERVICE, "executorMessenger_ is null.");
    }
}

FIRetCode FaceAuthManager::InitAlgorithm(std::string bundleName)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "Init, bundleName:%{public}s", bundleName.c_str());
    AlgoResult result = IsNeedAlgoLoad(bundleName);
    if (result == AR_EMPTY) {
        std::promise<int32_t> promiseobj;
        std::future<int32_t> futureobj = promiseobj.get_future();
        FaceAuthThreadPool::GetInstance()->AddTask(
            [&promiseobj]() { promiseobj.set_value(FaceAuthCA::GetInstance()->LoadAlgorithm()); });
        std::chrono::microseconds span(INIT_DYNAMIC_TIME_OUT);
        while (futureobj.wait_for(span) == std::future_status::timeout) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "LoadAlgorithm TimeOut");
            return FI_RC_ERROR;
        }
        return static_cast<FIRetCode>(futureobj.get());
    }
    FACEAUTH_HILOGE(MODULE_SERVICE, "Init Fail %{public}d", result);
    return FI_RC_ERROR;
}

FIRetCode FaceAuthManager::ReleaseAlgorithm(std::string bundleName)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "Release, bundleName:%{public}s", bundleName.c_str());
    AlgoResult result = IsNeedAlgoRelease(bundleName);
    if (result == AR_EMPTY) {
        std::promise<int32_t> promiseobj;
        std::future<int32_t> futureobj = promiseobj.get_future();
        FaceAuthThreadPool::GetInstance()->AddTask(
            [&promiseobj]() { promiseobj.set_value(FaceAuthCA::GetInstance()->ReleaseAlgorithm()); });
        std::chrono::microseconds span(RELEASE_DYNAMIC_TIME_OUT);
        while (futureobj.wait_for(span) == std::future_status::timeout) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "ReleaseAlgorithm TimeOut");
            return FI_RC_ERROR;
        }
        return static_cast<FIRetCode>(futureobj.get());
    }
    FACEAUTH_HILOGE(MODULE_SERVICE, "Release Fail %{public}d", result);
    return FI_RC_ERROR;
}

bool FaceAuthManager::IsAlgorithmInited()
{
    if (bundleNameList_.empty()) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthAlgoDispatcher IsInited bundleNameList_ is empty, need init");
        return true;
    }
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthAlgoDispatcher IsInited bundleNameList_ is not empty, no need init");
    return false;
}

AlgoResult FaceAuthManager::IsNeedAlgoLoad(std::string bundleName)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthAlgoDispatcher IsNeedAlgoLoad start");
    AlgoResult result = AR_SUCCESS;

    if (bundleNameList_.empty()) {
        result = AR_EMPTY;
    }

    if (bundleNameList_.find(bundleName) != bundleNameList_.end()) {
        bundleNameList_[bundleName] = bundleNameList_[bundleName] + 1;
        FACEAUTH_HILOGI(MODULE_SERVICE,
            "Add same bundleName:%{public}s, num:%{public}d", bundleName.c_str(), bundleNameList_[bundleName]);
        return AR_ADD_AGAIN;
    }

    bundleNameList_.insert(std::pair<std::string, int32_t>(bundleName, 1));

    FACEAUTH_HILOGI(MODULE_SERVICE, "Insert bundleName:%{public}s", bundleName.c_str());
    FACEAUTH_HILOGI(MODULE_SERVICE, "Result:%{public}d", result);
    return result;
}

AlgoResult FaceAuthManager::IsNeedAlgoRelease(std::string bundleName)
{
    AlgoResult result = AR_SUCCESS;
    if (bundleNameList_.erase(bundleName) != 0) {
        if (bundleNameList_.empty()) {
            result = AR_EMPTY;
        }
        FACEAUTH_HILOGI(MODULE_SERVICE, "Remove Success bundleName:%{public}s", bundleName.c_str());
    } else {
        result = AR_NOT_FOUND;
        FACEAUTH_HILOGE(MODULE_SERVICE, "Remove Fail bundleName:%{public}s", bundleName.c_str());
    }
    return result;
}

int32_t FaceAuthManager::GenerateEventId()
{
    int32_t randomNum = INVALID_EVENT_ID;
    if (!GetRandomNum(&randomNum)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetRandomNum error");
        return INVALID_EVENT_ID;
    }
    int32_t eventId = randomNum;
    FACEAUTH_HILOGI(MODULE_SERVICE, "GenerateEventId generate eventId %{public}u", eventId);
    return eventId;
}
int32_t FaceAuthManager::OpenCamera(sptr<IBufferProducer> producer)
{
    std::promise<int32_t> promiseobj;
    std::future<int32_t> futureobj = promiseobj.get_future();
    FaceAuthThreadPool::GetInstance()->AddTask([&promiseobj, &producer]() {
        promiseobj.set_value(FaceAuthCamera::GetInstance()->OpenCamera(producer));
    });
    std::chrono::microseconds span(OPEN_CAMERA_TIME_OUT);
    while (futureobj.wait_for(span) == std::future_status::timeout) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Open Camera TimeOut");
        return FA_RET_ERROR;
    }
    if (futureobj.get() == FA_RET_ERROR) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Open Camera Fail");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

int32_t FaceAuthManager::WaitAlgorithmProcessDone(uint64_t scheduleID)
{
    std::promise<int32_t> promiseobj;
    std::future<int32_t> futureobj = promiseobj.get_future();
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthCurTaskNum is %{public}d",
        FaceAuthThreadPool::GetInstance()->GetCurTaskNum());
    FaceAuthThreadPool::GetInstance()->AddTask([&promiseobj, this, &scheduleID]() {
        promiseobj.set_value(OperForAlgorithm(scheduleID));
    });
    std::chrono::microseconds span(GET_RESULT_TIME_OUT);
    while (futureobj.wait_for(span) == std::future_status::timeout) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "GetAuthResult TimeOut");
        return FA_RET_ERROR;
    }
    if (futureobj.get() != FI_RC_OK) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "GetAuthResult Fail");
        return FA_RET_ERROR;
    }
    return FA_RET_OK;
}

bool FaceAuthManager::GetRandomNum(int32_t *randomNum)
{
    if (randomNum == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "randomNum is nullptr.");
        return false;
    }
    BIGNUM *bn = BN_new();
    if (bn == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "BN_new fail.");
        return false;
    }
    if (BN_rand(bn, RAND_NUM_BITS, TOP, BOTTOM) == 0) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "BN_rand fail.");
        BN_free(bn);
        return false;
    }
    char *decVal = BN_bn2dec(bn);
    if (decVal == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "BN_bn2dec is nullptr.");
        BN_free(bn);
        return false;
    }
    *randomNum = atoi(decVal);
    BN_free(bn);
    return true;
}

FIRetCode FaceAuthManager::DoWaitInitAlgorithm(std::future<int32_t> futureobj)
{
    std::chrono::microseconds span(INIT_DYNAMIC_TIME_OUT);
    while (futureobj.wait_for(span) == std::future_status::timeout) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "LoadAlgorithm TimeOut");
        return FI_RC_ERROR;
    }
    return static_cast<FIRetCode>(futureobj.get());
}

void FaceAuthManager::UnfreezeTemplates(std::vector<uint64_t> templateIdList)
{
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get FaceAuthCA instance failed.");
        return;
    }

    for (auto templateId : templateIdList) {
        if (faceAuthCA->ResetRemainTimes(templateId) != FA_RET_OK) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "resetRemainTimes failed.");
        }
    }
}

void FaceAuthManager::FreezeTemplates(std::vector<uint64_t> templateIdList)
{
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "get FaceAuthCA instance failed.");
        return;
    }

    for (auto templateId : templateIdList) {
        if (faceAuthCA->FreezeTemplate(templateId) != FA_RET_OK) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FreezeTemplate failed.");
        }
    }
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
