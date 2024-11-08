/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

import hilog from '@ohos.hilog';

const TAG = 'Settings_FaceAuth';
const IAM_DOMAIN_ID_FACE = 0x2406;

export default class Log {

  static info(tag, content): void {
    hilog.info(IAM_DOMAIN_ID_FACE, TAG + ':' + tag, content,);
  }

  static error(tag, content): void {
    hilog.error(IAM_DOMAIN_ID_FACE, TAG + ':' + tag, content,);
  }
}