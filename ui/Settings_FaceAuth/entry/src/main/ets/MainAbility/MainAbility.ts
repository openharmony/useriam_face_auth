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

import Ability from '@ohos.app.ability.UIAbility'
import Log from '../utils/log'
import UserIdmModel from '../model/userIdmModel'
import CommonController from '../controller/commonController'
import EnrollingController from '../controller/enrollingController'

export default class MainAbility extends Ability {
  private readonly TAG: string = 'MainAbility:';
  onCreate(want, launchParam) {
    Log.info(this.TAG, 'Application onCreate')
    globalThis.abilityContext = this.context;
    globalThis.abilityWant = want;
  }

  onDestroy() {
    Log.info(this.TAG, 'Application onDestroy+')
    EnrollingController.clear()
    UserIdmModel.destroy()
    Log.info(this.TAG, 'Application onDestroy-')
  }

  onWindowStageCreate(windowStage) {
    Log.info(this.TAG, 'onWindowStageCreate')
    windowStage.setUIContent(this.context, "pages/entryView", null)
  }

  onWindowStageDestroy(): void {
    Log.info(this.TAG, 'onWindowStageDestroy')
  }

  onBackground(): void {
    Log.info(this.TAG, 'Application onBackground+, terminate ability')
    CommonController.terminateAbility()
    Log.info(this.TAG, 'Application onBackground-')
  }
}