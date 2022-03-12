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

#include "buffer.h"
#include "securec.h"
#include "face_auth_log_wrapper.h"
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const int MAX_BUFFER_SIZE = 512000;

bool IsBufferValid(const Buffer *buffer)
{
    if ((buffer == nullptr) || (buffer->buf == nullptr) ||
        (buffer->maxSize == 0) || (buffer->maxSize > MAX_BUFFER_SIZE) ||
        (buffer->contentSize > buffer->maxSize)) {
        return false;
    }

    return true;
}

bool CheckBufferWithSize(const Buffer *buffer, const uint32_t size)
{
    if ((!IsBufferValid(buffer)) || (buffer->contentSize != size)) {
        return false;
    }

    return true;
}

Buffer *CreateBuffer(const uint32_t size)
{
    if ((size == 0) || (size > MAX_BUFFER_SIZE)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Bad param size:%u", size);
        return nullptr;
    }

    Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
    if (buffer == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Get buffer struct error");
        return nullptr;
    }

    buffer->buf = (uint8_t *)malloc(size);
    if (buffer->buf == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Get buffer error");
        free(buffer);
        return nullptr;
    }

    if (memset_s(buffer->buf, size, 0, size) != EOK) {
        free(buffer->buf);
        free(buffer);
        return nullptr;
    }
    buffer->maxSize = size;
    buffer->contentSize = 0;

    return buffer;
}

ResultCode InitBuffer(Buffer *buffer, const uint8_t *buf, const uint32_t bufSize)
{
    if (!IsBufferValid(buffer) || (buf == nullptr) || (bufSize == 0)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Bad param");
        return RESULT_BAD_PARAM;
    }

    if (memcpy_s(buffer->buf, buffer->maxSize, buf, bufSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Copy buffer fail");
        return RESULT_BAD_COPY;
    }
    buffer->contentSize = bufSize;

    return RESULT_SUCCESS;
}

void DestoryBuffer(Buffer *buffer)
{
    if (buffer != nullptr) {
        if (buffer->buf != nullptr) {
            if (memset_s(buffer->buf, buffer->contentSize, 0, buffer->contentSize) != EOK) {
                FACEAUTH_HILOGE(MODULE_SERVICE, "DestoryBuffer memset fail!");
            }
            free(buffer->buf);
            buffer->buf = nullptr;
            buffer->contentSize = 0;
            buffer->maxSize = 0;
        }
        free(buffer);
    }
}

Buffer *CopyBuffer(const Buffer *buffer)
{
    if (!IsBufferValid(buffer)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Invalid buffer");
        return nullptr;
    }

    Buffer *copyBuffer = CreateBuffer(buffer->maxSize);
    if (copyBuffer == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Invalid buffer");
        return nullptr;
    }

    if (memcpy_s(copyBuffer->buf, copyBuffer->maxSize, buffer->buf, buffer->contentSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Copy buffer fail");
        DestoryBuffer(copyBuffer);
        return nullptr;
    }
    copyBuffer->contentSize = buffer->contentSize;

    return copyBuffer;
}

bool CompareBuffer(const Buffer *buffer1, const Buffer *buffer2)
{
    if (!IsBufferValid(buffer1) || !IsBufferValid(buffer2) || (buffer1->contentSize != buffer2->contentSize)) {
        return false;
    }

    if (memcmp(buffer1->buf, buffer2->buf, buffer1->contentSize) == 0) {
        return true;
    }

    return false;
}

ResultCode GetBufferData(const Buffer *buffer, uint8_t *data, uint32_t *dataSize)
{
    if (!IsBufferValid(buffer) || (data == nullptr) || (dataSize == nullptr)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Bad param");
        return RESULT_BAD_PARAM;
    }
    if (memcpy_s(data, *dataSize, buffer->buf, buffer->contentSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Copy buffer fail");
        return RESULT_BAD_COPY;
    }
    *dataSize = buffer->contentSize;
    return RESULT_SUCCESS;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS
