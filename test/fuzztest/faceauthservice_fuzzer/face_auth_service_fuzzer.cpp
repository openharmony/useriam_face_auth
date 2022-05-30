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

#include "face_auth_service_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "parcel.h"
#include "ibuffer_producer.h"

#include "iam_logger.h"

#include "face_auth_service.h"

#define LOG_LABEL UserIAM::Common::LABEL_FACE_AUTH_SA

#undef private

using namespace std;
using namespace OHOS::UserIAM::Common;

namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
namespace {
class DummyBufferProducer : public IBufferProducer {
public:
    ~DummyBufferProducer() = default;
    GSError RequestBuffer(
        const BufferRequestConfig &config, sptr<BufferExtraData> &bedata, RequestBufferReturnValue &retval)
    {
        return GSERROR_OK;
    }

    GSError CancelBuffer(int32_t sequence, const sptr<BufferExtraData> &bedata)
    {
        return GSERROR_OK;
    }

    GSError FlushBuffer(
        int32_t sequence, const sptr<BufferExtraData> &bedata, const sptr<SyncFence> &fence, BufferFlushConfig &config)
    {
        return GSERROR_OK;
    }

    GSError AttachBuffer(sptr<SurfaceBuffer> &buffer)
    {
        return GSERROR_OK;
    }

    GSError DetachBuffer(sptr<SurfaceBuffer> &buffer)
    {
        return GSERROR_OK;
    }

    uint32_t GetQueueSize()
    {
        return 0;
    }

    GSError SetQueueSize(uint32_t queueSize)
    {
        return GSERROR_OK;
    }

    GSError GetName(std::string &name)
    {
        return GSERROR_OK;
    }

    uint64_t GetUniqueId()
    {
        return 0;
    }

    GSError GetNameAndUniqueId(std::string &name, uint64_t &uniqueId)
    {
        return GSERROR_OK;
    }

    int32_t GetDefaultWidth()
    {
        return 0;
    }

    int32_t GetDefaultHeight()
    {
        return 0;
    }

    uint32_t GetDefaultUsage()
    {
        return 0;
    }

    GSError CleanCache()
    {
        return GSERROR_OK;
    }

    GSError RegisterReleaseListener(OnReleaseFunc func)
    {
        return GSERROR_OK;
    }

    GSError SetTransform(TransformType transform)
    {
        return GSERROR_OK;
    }

    GSError IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos, std::vector<bool> &supporteds)
    {
        return GSERROR_OK;
    }

    GSError Disconnect()
    {
        return GSERROR_OK;
    }

    sptr<IRemoteObject> AsObject()
    {
        return nullptr;
    }
};

auto g_service = FaceAuthService::GetInstance();

void FuzzOnStart(Parcel &parcel)
{
    IAM_LOGI("begin");
    g_service->OnStart();
    IAM_LOGI("end");
}

void FuzzOnStop(Parcel &parcel)
{
    IAM_LOGI("begin");
    g_service->OnStop();
    IAM_LOGI("end");
}

void FuzzSetBufferProducer(Parcel &parcel)
{
    IAM_LOGI("begin");
    sptr<IBufferProducer> bufferProducer = nullptr;
    if (parcel.ReadBool()) {
        bufferProducer = new (std::nothrow) DummyBufferProducer();
    }
    g_service->SetBufferProducer(bufferProducer);
    IAM_LOGI("end");
}

using FuzzFunc = decltype(FuzzOnStart);
FuzzFunc *g_fuzzFuncs[] = {FuzzOnStart, FuzzOnStop, FuzzSetBufferProducer};

void FaceAuthServiceFuzzTest(const uint8_t *data, size_t size)
{
    Parcel parcel;
    parcel.WriteBuffer(data, size);
    parcel.RewindRead(0);
    uint32_t index = parcel.ReadUint32() % (sizeof(g_fuzzFuncs) / sizeof(FuzzFunc *));
    auto fuzzFunc = g_fuzzFuncs[index];
    fuzzFunc(parcel);
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
