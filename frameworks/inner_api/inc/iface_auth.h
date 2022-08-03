/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef IFACE_AUTH_H
#define IFACE_AUTH_H
#include <list>
#include <vector>

#include "iremote_broker.h"
#include "iremote_object.h"
#include "surface.h"

#include "face_auth_defines.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class IFaceAuth : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.faceauth.IFaceAuth");

    /**
     * @brief Set buffer producer for enroll preview.
     *
     * @return 0 success, others failure.
     */
    virtual int32_t SetBufferProducer(sptr<IBufferProducer> &producer) = 0;

    enum {
        FACE_AUTH_SET_BUFFER_PRODUCER = 1,
    };
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // IFACE_AUTH_H