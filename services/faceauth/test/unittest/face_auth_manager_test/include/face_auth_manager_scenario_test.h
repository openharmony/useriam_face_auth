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
#ifndef FACE_AUTH_MANAGER_SCENARIO_TEST_H
#define FACE_AUTH_MANAGER_SCENARIO_TEST_H
#include <iostream>
#include <fstream>
#include <thread>
#include "gtest/gtest.h"
#include "ability_manager_interface.h"
#include "faceauth_log_wrapper.h"
#include "constant.h"
#include "iface_auth.h"
#include "input/camera_manager.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "face_auth_service.h"
#include "face_auth_defines.h"
#include "on_faceauth_stub.h"
#include "mock_bundle_mgr.h"
#include "surface.h"

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace {
static std::shared_ptr<FaceAuthService> service;
}
class TestCallback : public OnFaceAuthStub {
public:
    TestCallback() = default;
    virtual ~TestCallback() = default;

    void OnCallbackEvent(uint64_t reqId, int32_t type, int32_t code, int32_t errorCode, Bundle &bundle)
    {
        reqid_ = reqId;
        type_ = type;
        code_ = code;
        errorcode_ = errorCode;
        if (code_ == CODE_CALLBACK_RESULT || code_ == CODE_CALLBACK_CANCEL) {
            isCallback_ = true;
        }
        FACEAUTH_LABEL_LOGI("reqid_ is %{public}d", reqid_);
        FACEAUTH_LABEL_LOGI("type_ is %{public}d", type_);
        FACEAUTH_LABEL_LOGI("code_ is %{public}d", code_);
        FACEAUTH_LABEL_LOGI("errorcode_ is %{public}d", errorcode_);
    }
    void OnDeathRecipient()
    {}
    int32_t reqid_;
    int32_t type_;
    int32_t code_;
    int32_t errorcode_;
    bool isCallback_ = false;
};
class FaceAuthScenarioTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        OHOS::sptr<OHOS::IRemoteObject> bundleObject = new OHOS::AppExecFwk::BundleMgrService();
        auto sysMgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sysMgr == NULL) {
            GTEST_LOG_(ERROR) << "fail to get ISystemAbilityManager";
            return;
        }
        sysMgr->AddSystemAbility(Constant::ServiceId::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
        service = DelayedSingleton<FaceAuthService>::GetInstance();
        service->OnStart();
        service->Init();
    }
    static void TearDownTestCase()
    {
        service->Release();
        service->OnStop();
    }
    void SetUp()
    {}
    void TearDown()
    {}
    void WriteFile(int32_t caseNum, int32_t codeNum, std::map<int32_t, ResultInfo> resultInfos_);
    void CPPAPI_Function_Scenario_0300_Auth(int32_t num, std::map<int32_t, ResultInfo> resultInfos_);
    void CPPAPI_Function_Scenario_0100_Check(OHOS::sptr<TestCallback> remoteCallback,
        OHOS::sptr<TestCallback> localCallback, OHOS::sptr<TestCallback> enrollCallback,
        OHOS::sptr<TestCallback> removeCallback);
    AuthParam CPPAPI_Function_Scenario_0400_LocalAuth();
    AuthParam CPPAPI_Function_Scenario_0400_LocalAnotherAuth();
    AuthParam CPPAPI_Function_Scenario_0400_CoAuth();
    EnrollParam CPPAPI_Function_Scenario_0400_Enroll();
    RemoveParam CPPAPI_Function_Scenario_0400_Remove();
    class FaceAuthCameraBufferListener : public IBufferConsumerListener {
    public:
        sptr<Surface> cameraBuffer_;
        void OnBufferAvailable();
    };
};
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
#endif // FACE_AUTH_MANAGER_SCENARIO_TEST_H