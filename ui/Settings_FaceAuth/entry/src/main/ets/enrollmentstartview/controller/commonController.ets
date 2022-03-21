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

import Log from '../utils/log.ets'
import router from '@system.router';
import FeatureAbility from '@ohos.ability.featureAbility';
import Config from '../config/config.ets'
import UserIDMModel from '../model/userIDMModel.ets'

class CommonController {
  private TAG: string = "CommonController:"

  async routeBack() {
    if (router.getLength() > 1) {
      Log.info(this.TAG, 'router back: back+')
      router.back()
      Log.info(this.TAG, 'router back: back-')
    } else {
      Log.info(this.TAG, 'router back: terminate ability+')
      this.terminateAbility()
      Log.info(this.TAG, 'router back: terminate ability-')
    }
  }

  async terminateAbility() {
    Log.info(this.TAG, 'terminate ability+')
    UserIDMModel.destroy()
    FeatureAbility.terminateAbility()
    Log.info(this.TAG, 'terminate ability-')
  }

  async sleepMS(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

  getDialogAlignment():DialogAlignment {
    if (Config.getDeviceType() === 'phone') {
      return DialogAlignment.Bottom
    } else {
      return DialogAlignment.Center
    }
  }

  getHeightPercentageBegin(width: number, height: number) :string {
    var a = width / 2
    var b = height / 2
    var R = Math.sqrt(a * a + b * b)
    var H = R / 0.3
    var HPercentage = "" + Math.trunc(H * 166 / height) + '%'
    return HPercentage
  }

  setNonAppBarDisplaySize(nonAppBarDisplayWidth: number, nonAppBarDisplayHeight: number) {
    if (Config.getDeviceType() != 'phone') {
      return;
    }
    AppStorage.SetOrCreate('enrollImageHeight', nonAppBarDisplayHeight / 2);
    AppStorage.SetOrCreate('enrollImageWidth', nonAppBarDisplayWidth);
    // when percentage is 166%, circular diameter == height
    AppStorage.SetOrCreate('stackShelterHeightBegin',
      this.getHeightPercentageBegin(nonAppBarDisplayWidth, nonAppBarDisplayHeight / 2));
    AppStorage.SetOrCreate('stackShelterHeightEnd', "133%"); // 166 * 0.8
    AppStorage.SetOrCreate('stackRingHeightPercentage', "85%");
  }

  setDisplaySize(displayWidth: number, displayHeight: number) {
    if (Config.getDeviceType() != 'tablet') {
      return;
    }
    var sideLength = Math.min(displayWidth, displayHeight / 2) * 0.8
    AppStorage.SetOrCreate('enrollImageHeight', sideLength);
    AppStorage.SetOrCreate('enrollImageWidth', sideLength);
    AppStorage.SetOrCreate('stackShelterHeightBegin',
      this.getHeightPercentageBegin(sideLength, sideLength));
    AppStorage.SetOrCreate('stackShelterHeightEnd', "146%"); // 166 * 0.88
    AppStorage.SetOrCreate('stackRingHeightPercentage', "94%");
  }
}

let commonController = new CommonController();
export default commonController as CommonController