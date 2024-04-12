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

#include "face_auth_all_in_one_executor_hdi_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "parcel.h"
#include "refbase.h"

#include "iam_check.h"
#include "iam_fuzz_test.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "face_auth_all_in_one_executor_hdi.h"

#define LOG_TAG "FACE_AUTH_SA"

#undef private

using namespace std;
using namespace OHOS::UserIam::Common;
using namespace OHOS::UserIam::UserAuth;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace {
constexpr uint32_t MAX_VECTOR_LEN = 100;
class DummyExecutorProxy : public IAllInOneExecutor {
public:
    DummyExecutorProxy() : fuzzParcel_(nullptr)
    {
    }

    virtual ~DummyExecutorProxy() = default;

    int32_t GetExecutorInfo(ExecutorInfo &executorInfo)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        FillFuzzHdiExecutorInfo(*fuzzParcel_, executorInfo);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Enroll(uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
        const sptr<IExecutorCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Authenticate(uint64_t scheduleId, const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &extraInfo, const sptr<IExecutorCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t Identify(uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
        const sptr<IExecutorCallback> &callbackObj)
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

    int32_t SendCommand(int32_t commandId, const std::vector<uint8_t> &extraInfo,
        const sptr<IExecutorCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t SendMessage(uint64_t scheduleId, int32_t srcRole, const std::vector<uint8_t> &msg)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t GetProperty(const std::vector<uint64_t> &templateIdList, const std::vector<int32_t> &propertyTypes,
        Property &property)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        FillFuzzHdiProperty(*fuzzParcel_, property);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t SetCachedTemplates(const std::vector<uint64_t> &templateIdList)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        return (*fuzzParcel_).ReadInt32();
    }

    int32_t RegisterSaCommandCallback(const sptr<ISaCommandCallback> &callbackObj)
    {
        IF_FALSE_LOGE_AND_RETURN_VAL(fuzzParcel_ != nullptr, 0);
        callbackObj_ = callbackObj;
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

    void FuzzTriggerSaCommandCallback(Parcel &parcel)
    {
        if (callbackObj_ == nullptr) {
            return;
        }

        std::vector<SaCommand> commands;
        FillFuzzSaCommandVector(parcel, commands);
        callbackObj_->OnSaCommands(commands);
    }

private:
    void FillFuzzHdiExecutorInfo(Parcel &parcel, ExecutorInfo &executorInfo)
    {
        executorInfo.sensorId = parcel.ReadUint16();
        executorInfo.executorMatcher = parcel.ReadUint32();
        executorInfo.executorRole = static_cast<ExecutorRole>(parcel.ReadInt32());
        executorInfo.authType = static_cast<AuthType>(parcel.ReadInt32());
        executorInfo.esl = static_cast<ExecutorSecureLevel>(parcel.ReadInt32());
        executorInfo.maxTemplateAcl = parcel.ReadInt32();
        FillFuzzUint8Vector(parcel, executorInfo.publicKey);
        FillFuzzUint8Vector(parcel, executorInfo.extraInfo);
        IAM_LOGI("success");
    }

    void FillFuzzHdiProperty(Parcel &parcel, Property &property)
    {
        property.authSubType = parcel.ReadUint64();
        property.lockoutDuration = parcel.ReadInt32();
        property.remainAttempts = parcel.ReadInt32();
        FillFuzzString(parcel, property.enrollmentProgress);
        FillFuzzString(parcel, property.sensorInfo);

        IAM_LOGI("success");
    }

    void FillFuzzSaCommand(Parcel &parcel, SaCommand &command)
    {
        command.id = static_cast<SaCommandId>(parcel.ReadInt32());
        IAM_LOGI("success");
    }

    void FillFuzzSaCommandVector(Parcel &parcel, std::vector<SaCommand> &commands)
    {
        uint32_t len = parcel.ReadUint32() % MAX_VECTOR_LEN;
        commands.resize(len);
        for (uint32_t i = 0; i < len; i++) {
            FillFuzzSaCommand(parcel, commands[i]);
        }
        IAM_LOGI("success");
    }

    Parcel *fuzzParcel_;
    sptr<ISaCommandCallback> callbackObj_ { nullptr };
};

class DummyExecuteCallback : public UserAuth::IExecuteCallback {
public:
    virtual ~DummyExecuteCallback() = default;

    void OnResult(ResultCode result, const std::vector<uint8_t> &extraInfo) override
    {
    }

    void OnResult(ResultCode result) override
    {
    }

    void OnAcquireInfo(int32_t acquire, const std::vector<uint8_t> &extraInfo) override
    {
    }

    void OnMessage(int destRole, const std::vector<uint8_t> &msg) override
    {}
};

auto g_proxy = new (nothrow) DummyExecutorProxy();
auto g_hdi = Common::MakeShared<FaceAuthAllInOneExecutorHdi>(g_proxy);

void FillFuzzExecutorInfo(Parcel &parcel, UserAuth::ExecutorInfo &executorInfo)
{
    executorInfo.executorSensorHint = parcel.ReadInt32();
    executorInfo.authType = static_cast<UserAuth::AuthType>(parcel.ReadInt32());
    executorInfo.executorRole = static_cast<UserAuth::ExecutorRole>(parcel.ReadInt32());
    executorInfo.executorMatcher = parcel.ReadInt32();
    executorInfo.esl = static_cast<UserAuth::ExecutorSecureLevel>(parcel.ReadInt32());
    FillFuzzUint8Vector(parcel, executorInfo.publicKey);
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

void FillFuzzAttributeKeyVector(Parcel &parcel, std::vector<UserAuth::Attributes::AttributeKey> &keys)
{
    std::vector<uint32_t> vals;
    FillFuzzUint32Vector(parcel, vals);
    for (const auto &val : vals) {
        keys.push_back(static_cast<UserAuth::Attributes::AttributeKey>(val));
    }

    IAM_LOGI("success");
}

void FuzzGetExecutorInfo(Parcel &parcel)
{
    IAM_LOGI("begin");
    UserAuth::ExecutorInfo info;
    FillFuzzExecutorInfo(parcel, info);
    g_hdi->GetExecutorInfo(info);
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
    g_hdi->OnRegisterFinish(templateIdList, frameworkPublicKey, extraInfo);
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
    g_hdi->Enroll(scheduleId, EnrollParam { tokenId, extraInfo }, callbackObj);
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
    bool endAfterFirstFail = parcel.ReadBool();
    g_hdi->Authenticate(scheduleId, AuthenticateParam { tokenId, templateIdList, extraInfo, endAfterFirstFail },
        callbackObj);
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
    g_hdi->Identify(scheduleId, IdentifyParam { tokenId, extraInfo }, callbackObj);
    IAM_LOGI("end");
}

void FuzzDelete(Parcel &parcel)
{
    IAM_LOGI("begin");
    std::vector<uint64_t> templateIdList;
    FillFuzzUint64Vector(parcel, templateIdList);
    g_hdi->Delete(templateIdList);
    IAM_LOGI("end");
}

void FuzzCancel(Parcel &parcel)
{
    IAM_LOGI("begin");
    uint64_t scheduleId = parcel.ReadUint64();
    g_hdi->Cancel(scheduleId);
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
    g_hdi->SendCommand(commandId, extraInfo, callbackObj);
    IAM_LOGI("end");
}

void FuzzGetProperty(Parcel &parcel)
{
    IAM_LOGI("begin");
    std::vector<uint64_t> templateIdList;
    FillFuzzUint64Vector(parcel, templateIdList);
    std::vector<UserAuth::Attributes::AttributeKey> keys;
    FillFuzzAttributeKeyVector(parcel, keys);
    UserAuth::Property property;
    g_hdi->GetProperty(templateIdList, keys, property);
    IAM_LOGI("end");
}

void FuzzSetCachedTemplates(Parcel &parcel)
{
    IAM_LOGI("begin");
    std::vector<uint64_t> templateIdList;
    FillFuzzUint64Vector(parcel, templateIdList);
    g_hdi->SetCachedTemplates(templateIdList);
    IAM_LOGI("end");
}

void FuzzSendMessage(Parcel &parcel)
{
    IAM_LOGI("begin");
    IAM_LOGI("end");
}

void FuzzTriggerSaCommandCallback(Parcel &parcel)
{
    IAM_LOGI("begin");
    g_proxy->FuzzTriggerSaCommandCallback(parcel);
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
FuzzFunc *g_fuzzFuncs[] = { FuzzGetExecutorInfo, FuzzOnRegisterFinish, FuzzEnroll, FuzzAuthenticate, FuzzIdentify,
    FuzzDelete, FuzzCancel, FuzzSendCommand, FuzzGetProperty, FuzzSetCachedTemplates, FuzzTriggerSaCommandCallback,
    FuzzSendMessage };

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
} // namespace UserIam
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int32_t LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::UserIam::FaceAuth::FaceAuthServiceFuzzTest(data, size);
    return 0;
}
