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

#include "face_auth_executor_hdi_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <vector>

#include "parcel.h"
#include "refbase.h"

#include "iam_check.h"
#include "iam_fuzz_test.h"
#include "iam_logger.h"
#include "iam_ptr.h"
#include "framework_types.h"
#include "iexecute_callback.h"

#include "face_auth_executor_hdi.h"
#include "v1_0/face_auth_types.h"
#include "v1_0/iexecutor.h"

#define LOG_LABEL UserIam::Common::LABEL_FACE_AUTH_SA

#undef private

using namespace std;
using namespace OHOS::UserIAM::Common;
using namespace OHOS::UserIam::UserAuth;
namespace FaceHdi = OHOS::HDI::FaceAuth::V1_0;

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace {
class DummyExecutorProxy : public FaceHdi::IExecutor {
public:
    DummyExecutorProxy() : fuzzParcel_(nullptr)
    {
    }

    virtual ~DummyExecutorProxy() = default;

    int32_t GetExecutorInfo(FaceHdi::ExecutorInfo &executorInfo)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        FillFuzzHdiExecutorInfo(*fuzzParcel_, executorInfo);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t GetTemplateInfo(uint64_t templateId, FaceHdi::TemplateInfo &templateInfo)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        FillFuzzHdiTemplateInfo(*fuzzParcel_, templateInfo);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Enroll(
        uint64_t scheduleId, const std::vector<uint8_t> &extraInfo, const sptr<FaceHdi::IExecutorCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Authenticate(uint64_t scheduleId, const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &extraInfo, const sptr<FaceHdi::IExecutorCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Identify(
        uint64_t scheduleId, const std::vector<uint8_t> &extraInfo, const sptr<FaceHdi::IExecutorCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Delete(const std::vector<uint64_t> &templateIdList)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Cancel(uint64_t scheduleId)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t SendCommand(
        int32_t commandId, const std::vector<uint8_t> &extraInfo, const sptr<FaceHdi::IExecutorCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    void SetParcel(Parcel &parcel)
    {
        fuzzParcel_ = &parcel;
    }

    void ClearParcel()
    {
        fuzzParcel_ = nullptr;
    }

private:
    void FillFuzzHdiExecutorInfo(Parcel &parcel, FaceHdi::ExecutorInfo &executorInfo)
    {
        executorInfo.sensorId = parcel.ReadUint16();
        executorInfo.executorType = parcel.ReadUint32();
        executorInfo.executorRole = static_cast<FaceHdi::ExecutorRole>(parcel.ReadInt32());
        executorInfo.authType = static_cast<FaceHdi::AuthType>(parcel.ReadInt32());
        executorInfo.esl = static_cast<FaceHdi::ExecutorSecureLevel>(parcel.ReadInt32());
        FillFuzzUint8Vector(parcel, executorInfo.publicKey);
        FillFuzzUint8Vector(parcel, executorInfo.extraInfo);
        IAM_LOGI("success");
    }

    void FillFuzzHdiTemplateInfo(Parcel &parcel, FaceHdi::TemplateInfo &templateInfo)
    {
        templateInfo.executorType = parcel.ReadUint32();
        templateInfo.freezingTime = parcel.ReadInt32();
        templateInfo.remainTimes = parcel.ReadInt32();
        FillFuzzUint8Vector(parcel, templateInfo.extraInfo);
        IAM_LOGI("success");
    }

    Parcel *fuzzParcel_;
};

class DummyExecuteCallback : public UserAuth::IExecuteCallback {
public:
    virtual ~DummyExecuteCallback() = default;

    void OnResult(ResultCode result, const std::vector<uint8_t> &extraInfo)
    {
    }

    void OnResult(ResultCode result)
    {
    }

    void OnAcquireInfo(int32_t acquire, const std::vector<uint8_t> &extraInfo)
    {
    }
};

auto g_proxy = new (nothrow) DummyExecutorProxy();
FaceAuthExecutorHdi g_hdi(g_proxy);

void FillFuzzExecutorInfo(Parcel &parcel, ExecutorInfo &executorInfo)
{
    executorInfo.executorSensorHint = parcel.ReadInt32();
    executorInfo.authType = static_cast<AuthType>(parcel.ReadInt32());
    executorInfo.executorRole = static_cast<ExecutorRole>(parcel.ReadInt32());
    executorInfo.executorMatcher = parcel.ReadInt32();
    executorInfo.esl = static_cast<ExecutorSecureLevel>(parcel.ReadInt32());
    FillFuzzUint8Vector(parcel, executorInfo.publicKey);
    IAM_LOGI("success");
}

void FillFuzzTemplateInfo(Parcel &parcel, UserAuth::TemplateInfo &templateInfo)
{
    templateInfo.executorType = parcel.ReadUint32();
    templateInfo.freezingTime = parcel.ReadInt32();
    templateInfo.remainTimes = parcel.ReadInt32();
    FillFuzzUint8Vector(parcel, templateInfo.extraInfo);
    IAM_LOGI("success");
}

void FillFuzzIExecuteCallback(Parcel &parcel, std::shared_ptr<UserAuth::IExecuteCallback> &callback)
{
    callback = nullptr;
    if (parcel.ReadBool()) {
        callback = MakeShared<DummyExecuteCallback>();
        if (callback == nullptr) {
            IAM_LOGE("callback is nullptr");
        }
    }
    IAM_LOGI("success");
}

void FuzzGetExecutorInfo(Parcel &parcel)
{
    IAM_LOGI("begin");
    ExecutorInfo info;
    FillFuzzExecutorInfo(parcel, info);
    g_hdi.GetExecutorInfo(info);
    IAM_LOGI("end");
}

void FuzzGetTemplateInfo(Parcel &parcel)
{
    IAM_LOGI("begin");
    uint64_t templateId = parcel.ReadUint64();
    UserAuth::TemplateInfo info;
    FillFuzzTemplateInfo(parcel, info);
    g_hdi.GetTemplateInfo(templateId, info);
    IAM_LOGI("end");
}

void FuzzOnRegisterFinish(Parcel &parcel)
{
    IAM_LOGI("begin");
    std::vector<uint64_t> templateIdList;
    FillFuzzUint64Vector(parcel, templateIdList);
    std::vector<uint8_t> frameworkPublicKey;
    FillFuzzUint8Vector(parcel, frameworkPublicKey);
    std::vector<uint8_t> extraInfo;
    FillFuzzUint8Vector(parcel, extraInfo);
    g_hdi.OnRegisterFinish(templateIdList, frameworkPublicKey, extraInfo);
    IAM_LOGI("end");
}

void FuzzEnroll(Parcel &parcel)
{
    IAM_LOGI("begin");
    uint64_t scheduleId = parcel.ReadUint64();
    uint32_t tokenId = parcel.ReadUint32();
    std::vector<uint8_t> extraInfo;
    FillFuzzUint8Vector(parcel, extraInfo);
    std::shared_ptr<UserAuth::IExecuteCallback> callbackObj;
    FillFuzzIExecuteCallback(parcel, callbackObj);
    g_hdi.Enroll(scheduleId, tokenId, extraInfo, callbackObj);
    IAM_LOGI("end");
}

void FuzzAuthenticate(Parcel &parcel)
{
    IAM_LOGI("begin");
    uint64_t scheduleId = parcel.ReadUint64();
    uint32_t tokenId = parcel.ReadUint32();
    std::vector<uint64_t> templateIdList;
    FillFuzzUint64Vector(parcel, templateIdList);
    std::vector<uint8_t> extraInfo;
    FillFuzzUint8Vector(parcel, extraInfo);
    std::shared_ptr<UserAuth::IExecuteCallback> callbackObj;
    FillFuzzIExecuteCallback(parcel, callbackObj);
    g_hdi.Authenticate(scheduleId, tokenId, templateIdList, extraInfo, callbackObj);
    IAM_LOGI("end");
}

void FuzzIdentify(Parcel &parcel)
{
    IAM_LOGI("begin");
    uint64_t scheduleId = parcel.ReadUint64();
    uint32_t tokenId = parcel.ReadUint32();
    std::vector<uint8_t> extraInfo;
    FillFuzzUint8Vector(parcel, extraInfo);
    std::shared_ptr<UserAuth::IExecuteCallback> callbackObj;
    FillFuzzIExecuteCallback(parcel, callbackObj);
    g_hdi.Identify(scheduleId, tokenId, extraInfo, callbackObj);
    IAM_LOGI("end");
}

void FuzzDelete(Parcel &parcel)
{
    IAM_LOGI("begin");
    std::vector<uint64_t> templateIdList;
    FillFuzzUint64Vector(parcel, templateIdList);
    g_hdi.Delete(templateIdList);
    IAM_LOGI("end");
}

void FuzzCancel(Parcel &parcel)
{
    IAM_LOGI("begin");
    uint64_t scheduleId = parcel.ReadUint64();
    g_hdi.Cancel(scheduleId);
    IAM_LOGI("end");
}

void FuzzSendCommand(Parcel &parcel)
{
    IAM_LOGI("begin");
    PropertyMode commandId = static_cast<PropertyMode>(parcel.ReadInt32());
    std::vector<uint8_t> extraInfo;
    FillFuzzUint8Vector(parcel, extraInfo);
    std::shared_ptr<UserAuth::IExecuteCallback> callbackObj;
    FillFuzzIExecuteCallback(parcel, callbackObj);
    g_hdi.SendCommand(commandId, extraInfo, callbackObj);
    IAM_LOGI("end");
}

void SetProxyParcel(Parcel &parcel)
{
    if (g_proxy == nullptr) {
        IAM_LOGE("g_proxy is nullptr");
        return;
    }
    g_proxy->SetParcel(parcel);
}

void ClearProxyParcel()
{
    if (g_proxy == nullptr) {
        IAM_LOGE("g_proxy is nullptr");
        return;
    }
    g_proxy->ClearParcel();
}

using FuzzFunc = decltype(FuzzGetExecutorInfo);
FuzzFunc *g_fuzzFuncs[] = {FuzzGetExecutorInfo, FuzzGetTemplateInfo, FuzzOnRegisterFinish, FuzzEnroll, FuzzAuthenticate,
    FuzzIdentify, FuzzDelete, FuzzCancel, FuzzSendCommand};

void FaceAuthServiceFuzzTest(const uint8_t *data, size_t size)
{
    Parcel parcel;
    parcel.WriteBuffer(data, size);
    parcel.RewindRead(0);
    uint32_t index = parcel.ReadUint32() % (sizeof(g_fuzzFuncs) / sizeof(FuzzFunc *));
    auto fuzzFunc = g_fuzzFuncs[index];
    SetProxyParcel(parcel);
    fuzzFunc(parcel);
    ClearProxyParcel();
    return;
}
} // namespace
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int32_t LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::UserIAM::FaceAuth::FaceAuthServiceFuzzTest(data, size);
    return 0;
}
