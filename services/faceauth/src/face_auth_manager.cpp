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
#include "face_auth_manager.h"
#include <iservice_registry.h>
#include <thread>
#include "face_auth_log_wrapper.h"
#include "face_auth_event_handler.h"
#include "auth_executor_registry.h"
#include "auth_message.h"
#include "coauth_info_define.h"
#include "face_auth_thread_pool.h"
#include "face_auth_req.h"
#include "face_auth_camera.h"
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
std::shared_ptr<FaceAuthManager> FaceAuthManager::manager_ = nullptr;
std::mutex FaceAuthManager::mutex_;
sptr<AuthResPool::IExecutorMessenger> FaceAuthManager::executorMessenger_;
std::shared_ptr<FaceAuthEventHandler> FaceAuthManager::handler_ = nullptr;
std::shared_ptr<AppExecFwk::EventRunner> FaceAuthManager::runner_ = nullptr;
std::shared_ptr<AuthResPool::QueryCallback> FaceAuthManager::queryCallback_ = nullptr;
std::shared_ptr<FaceAuthExecutorCallback> FaceAuthManager::executorCallback_ = nullptr;

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

void CheckSystemAbility()
{
    const int CHECK_TIMES = 3;
    const int SLEEP_TIME = 1;
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Failed to get system ability manager");
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
    FACEAUTH_HILOGI(MODULE_SERVICE, "start AUTHEXECUTORMGR ability all failed");
}

int32_t FaceAuthManager::Init()
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    // run event handler
    std::string threadName("FaceAuthEventRunner");
    runner_ = AppExecFwk::EventRunner::Create(threadName);
    if (!runner_) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "failed to create a runner.");
        return FA_RET_ERROR;
    }
    runner_->Run();
    handler_ = std::make_shared<FaceAuthEventHandler>(runner_);
    if (!handler_) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "handler_ is nullpter.");
        return FA_RET_ERROR;
    }
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
    FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthManager::Init Success.");
    // check register state
    std::thread checkThread(OHOS::UserIAM::FaceAuth::CheckSystemAbility);
    checkThread.join();
    QueryRegStatus();
    return FA_RET_OK;
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
    int32_t iRet = faceAuthCA->GetExecutorInfo(pubKey, esl, authAbility);
    if (FA_RET_OK != iRet) {
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
    int32_t iRet = faceAuthCA->GetExecutorInfo(pubKey, esl, authAbility);
    if (FA_RET_OK != iRet) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "GetExecutorInfo failed.");
        return;
    }
    std::shared_ptr<AuthResPool::AuthExecutor> executorInfo = std::make_shared<AuthResPool::AuthExecutor>();
    // set executor info
    executorInfo->SetPublicKey(pubKey);
    executorInfo->SetExecutorSecLevel(ESL0);
    executorInfo->SetAuthAbility(authAbility);
    executorInfo->SetAuthType(FACE);
    executorInfo->SetExecutorType(TYPE_ALL_IN_ONE);
    executorCallback_ = std::make_shared<FaceAuthExecutorCallback>();
    // executorCallback_ release
    uint64_t ret = AuthResPool::AuthExecutorRegistry::GetInstance().Register(executorInfo, executorCallback_);
    if (ret != 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthInitSeq::RegisterExecutor successful.executor id = %{public}llu", ret);
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthInitSeq::RegisterExecutor failed.");
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

int32_t FaceAuthManager::Authenticate(const AuthParam &param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoAuth]scheduleID = %{public}llu.", param.scheduleID);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoAuth]templateID = %{public}llu.", param.templateID);
    // check param
    if (param.templateID < 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Parameter check error.");
        return FA_RET_ERROR;
    }

    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_LOCAL_AUTH)) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Auth is Max.");
        return FA_RET_ERROR;
    }
    // send event
    FaceInfo faceInfo;
    faceInfo.eventId = GenerateEventId();
    faceInfo.uId = param.callerUID;
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::LOW;
    auto authInfo = std::make_unique<AuthParam>(param);
    handler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return FA_RET_OK;
}
void FaceAuthManager::HandleCallAuthenticate(const AuthParam &param)
{
    // remove require info
    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
    // check param
    if (param.templateID < 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Parameter check error.");
        return;
    }
    this->InitAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
    // open camera and send image to algorithm
    if (OpenCamera() != FA_RET_OK) {
        // RK3568 no support camera, temporary ignore error
        FACEAUTH_HILOGI(MODULE_SERVICE, "Ignore open camera fail.");
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
    int32_t iRet = faceAuthCA->StartAlgorithmOperation(algorithmOperation, algorithmParam);
    if (iRet != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "StartAlgorithmOperation failed");
        return;
    }
    // receive algorithm message and handle algorithm result
    if (GetAuthMessage(param.scheduleID) != FA_RET_OK) {
        return;
    }
   // close camera
    std::shared_ptr<FaceAuthCamera> faceAuthCamera = FaceAuthCamera::GetInstance();
    if (faceAuthCamera == nullptr) {
        return;
    }
    faceAuthCamera->CloseCamera();
    this->ReleaseAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
}
int32_t FaceAuthManager::Enrollment(const EnrollParam &param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoEnroll]scheduleID = %{public}llu.", param.scheduleID);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoEnroll]templateID = %{public}llu.", param.templateID);
    // check param
    if (param.templateID < 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Parameter check error.");
        return FA_RET_ERROR;
    }

    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_ENROLL)) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Enroll is Max.");
        return FA_RET_ERROR;
    }
    // send event
    FaceInfo faceInfo;
    faceInfo.eventId = GenerateEventId();
    faceInfo.uId = param.callerUID;
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::HIGH;
    auto authInfo = std::make_unique<EnrollParam>(param);
    handler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return FA_RET_OK;
}
void FaceAuthManager::HandleCallEnroll(const EnrollParam &param)
{
    // remove require info
    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
    // check param
    if (param.templateID < 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Parameter check error.");
        return;
    }
    this->InitAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
    // open camera and send image to algorithm
    if (OpenCamera() != FA_RET_OK) {
        // RK3568 no support camera, temporary ignore error
        FACEAUTH_HILOGI(MODULE_SERVICE, "Ignore open camera fail.");
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
    int32_t iRet = faceAuthCA->StartAlgorithmOperation(algorithmOperation, algorithmParam);
    if (iRet != FA_RET_OK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "StartAlgorithmOperation failed");
        return;
    }
    // receive algorithm message and handle algorithm result
    if (GetAuthMessage(param.scheduleID) != FA_RET_OK) {
        return;
    }
    // close camera
    std::shared_ptr<FaceAuthCamera> faceAuthCamera = FaceAuthCamera::GetInstance();
    if (faceAuthCamera == nullptr) {
        return;
    }
    faceAuthCamera->CloseCamera();
    this->ReleaseAlgorithm(FACE_LOCAL_INIT_ALGO_NAME);
}
int32_t FaceAuthManager::Remove(const RemoveParam &param)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "%{public}s run.", __PRETTY_FUNCTION__);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoRemove]scheduleID = %{public}llu.", param.scheduleID);
    FACEAUTH_HILOGI(MODULE_SERVICE, "[DoRemove]templateID = %{public}llu.", param.templateID);
    // check param
    if (param.templateID < 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Parameter check error.");
        return FA_RET_ERROR;
    }

    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_DEL;
    if (FaceAuthReq::GetInstance()->IsReqNumReachedMax(FACE_OPERATE_TYPE_DEL)) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Remove is Max.");
        return FA_RET_ERROR;
    }
    // send event
    FaceInfo faceInfo;
    faceInfo.eventId = GenerateEventId();
    faceInfo.uId = param.callerUID;
    FaceAuthReq::GetInstance()->AddReqInfo(reqType, faceInfo);
    FaceAuthEventHandler::Priority priority = FaceAuthEventHandler::Priority::IMMEDIATE;
    auto authInfo = std::make_unique<RemoveParam>(param);
    handler_->SendEvent(faceInfo.eventId, std::move(authInfo), priority);
    return FA_RET_OK;
}
void FaceAuthManager::HandleCallRemove(const RemoveParam &param)
{
    // remove require info
    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_DEL;
    FaceAuthReq::GetInstance()->RemoveRequireInfo(reqType);
    // check param
    if (param.templateID < 0) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Parameter check error.");
        return;
    }
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "faceAuthCA instance is null.");
        return;
    }
    int32_t iRet = faceAuthCA->DeleteTemplete(param.templateID);
    if (FA_RET_OK == iRet) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "Remove success.");
    } else {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Remove failed.");
    }
}
FIRetCode FaceAuthManager::OperForAlgorithm(uint64_t scheduleID)
{
    std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
    if (faceAuthCA == nullptr) {
        FACEAUTH_HILOGI(MODULE_SERVICE, "faceAuthCA is null.");
        return FI_RC_INVALID_ARGUMENT;
    }
    int32_t retCode = 0;
    std::vector<uint8_t> retCoauthMsg;
    std::vector<uint8_t> m_msg;
    std::shared_ptr<AuthResPool::AuthMessage> msgInstance = std::make_shared<AuthResPool::AuthMessage>(m_msg);
    if (msgInstance == nullptr) {
        return FI_RC_INVALID_ARGUMENT;
    }
    while (1) {
        faceAuthCA->GetAlgorithmState(retCode, retCoauthMsg);
        FACEAUTH_HILOGI(MODULE_SERVICE, "receive new co auth message.");
        std::shared_ptr<AuthResPool::AuthMessage> msg(msgInstance->FromUint8Array(retCoauthMsg));
        if (msg != nullptr) {
            SendData(scheduleID, 0, TYPE_ALL_IN_ONE, TYPE_CO_AUTH, msg);
        }
        if (FACE_ALOGRITHM_OPERATION_BREAK == retCode) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "FACE_ALOGRITHM_OPERATION_BREAK.");
            break;
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
    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_LOCAL_AUTH;
    int32_t uId = param.callerUID;
    bool isSuccess = FaceAuthReq::GetInstance()->SetCancelFlag(reqType, uId);
    if (!isSuccess) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "CancelAuth failed,reqId: xxxx%04llu, ", reqType.reqId);
        result = FA_RET_ERROR;
    }
    if (isSuccess) {
        std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
        if (faceAuthCA == nullptr) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthCA instance is null");
            return result;
        }
        result = faceAuthCA->CancelAlogrithmOperation();
        if (result == FA_RET_OK) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "CancelAlogrithmOperation success");
        } else {
            FACEAUTH_HILOGE(MODULE_SERVICE, "CancelAlogrithmOperation failed");
        }
    }
    return result;
}

int32_t FaceAuthManager::CancelEnrollment(const EnrollParam &param)
{
    int32_t result = FA_RET_OK;
    FaceReqType reqType;
    reqType.reqId = param.scheduleID;
    reqType.operateType = FACE_OPERATE_TYPE_ENROLL;
    int32_t uId = param.callerUID;
    bool isSuccess = FaceAuthReq::GetInstance()->SetCancelFlag(reqType, uId);
    if (!isSuccess) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "CancelEnrollment failed,reqId: xxxx%04llu, ", reqType.reqId);
        result = FA_RET_ERROR;
    }
    if (isSuccess) {
        std::shared_ptr<FaceAuthCA> faceAuthCA = FaceAuthCA::GetInstance();
        if (faceAuthCA == nullptr) {
            FACEAUTH_HILOGE(MODULE_SERVICE, "FaceAuthCA instance is null");
            return result;
        }
        result = faceAuthCA->CancelAlogrithmOperation();
        if (result == FA_RET_OK) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "CancelAlogrithmOperation success");
        } else {
            FACEAUTH_HILOGE(MODULE_SERVICE, "CancelAlogrithmOperation failed");
        }
    }
    return result;
}

void FaceAuthManager::SetExecutorMessenger(const sptr<AuthResPool::IExecutorMessenger> &messager)
{
    executorMessenger_ = messager;
}
void FaceAuthManager::SendData(uint64_t scheduleId,
                               uint64_t transNum,
                               int32_t srcType,
                               int32_t dstType,
                               pAuthMessage msg)
{
    if (executorMessenger_ != nullptr) {
        executorMessenger_->SendData(scheduleId, transNum, srcType, dstType, msg);
    } else {
        FACEAUTH_HILOGI(MODULE_SERVICE, "executorMessenger_ is null.");
    }
}

void FaceAuthManager::Finish(uint64_t scheduleId,
                             int32_t srcType,
                             int32_t resultCode,
                             pAuthAttributes finalResult)
{
    if (executorMessenger_ != nullptr) {
        executorMessenger_->Finish(scheduleId, srcType, resultCode, finalResult);
    } else {
        FACEAUTH_HILOGI(MODULE_SERVICE, "executorMessenger_ is null.");
    }
}

FIRetCode FaceAuthManager::InitAlgorithm(std::string bundleName)
{
    FACEAUTH_HILOGI(MODULE_SERVICE, "Init ,bundleName:%{public}s", bundleName.c_str());
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
    FACEAUTH_HILOGI(MODULE_SERVICE, "Release ,bundleName:%{public}s", bundleName.c_str());
    AlgoResult result = IsNeedAlgoRelease(bundleName);
    if (result == AR_EMPTY) {
        if (FA_RET_OK == FaceAuthCA::GetInstance()->ReleaseAlgorithm()) {
            FACEAUTH_HILOGI(MODULE_SERVICE, "Release success");
            return FI_RC_OK;
        }
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
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthAlgoDispatcher IsInited bundleNameList_ is not empty,no need init");
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
            "Add same bundleName:%{public}s,num:%{public}d", bundleName.c_str(), bundleNameList_[bundleName]);
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
uint32_t FaceAuthManager::GenerateEventId()
{
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &t);
    int64_t elapsedTime { ((t.tv_sec) * SEC_TO_NANOSEC + t.tv_nsec) };
    size_t elapsedHash = std::hash<std::string>()(std::to_string(elapsedTime));
    uint32_t eventId = static_cast<uint32_t>(elapsedHash);
    FACEAUTH_HILOGI(MODULE_SERVICE, "GenerateEventId generate eventId %{public}u", eventId);
    return eventId;
}
int32_t FaceAuthManager::OpenCamera()
{
    std::promise<int32_t> promiseobj;
    std::future<int32_t> futureobj = promiseobj.get_future();
    FaceAuthThreadPool::GetInstance()->AddTask([&promiseobj]() {
        promiseobj.set_value(FaceAuthCamera::GetInstance()->OpenCamera(nullptr));
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
int32_t FaceAuthManager::GetAuthMessage(uint64_t scheduleID)
{
    std::promise<int32_t> promiseobj;
    std::future<int32_t> futureobj = promiseobj.get_future();
    FACEAUTH_HILOGI(MODULE_SERVICE, "FaceAuthCurTaskNum is %{public}d ",
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
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
