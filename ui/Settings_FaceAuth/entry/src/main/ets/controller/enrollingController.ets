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

import Constants from '../config/constant'
import Log from '../utils/log'
import router from '@system.router';
import CommonController from '../controller/commonController'
import UserIDMModel from '../model/userIDMModel'
import UserAuthModel from '../model/userAuthModel'

class EnrollingController {
  private TAG: string = "EnrollingController:"
  private ANIMATION_TIME: number = 2000
  private enrolling: boolean = false

  constructor() {
    Log.info(this.TAG, 'constructor+')
    Log.info(this.TAG, 'constructor-')
  }

  init() {
    Log.info(this.TAG, 'init+')
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Hidden);
    AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Visible);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);

    AppStorage.Set('stackProgressValue', 0);
    AppStorage.Set('stackShelterHeight', AppStorage.Get('stackShelterHeightBegin'));
    AppStorage.Set('enrollTip',  $r('app.string.face_intro'));
    AppStorage.Set('enrollTipSize', Constants.ohos_id_text_size_subtitle1);
    AppStorage.Set('stackVideoBlurRadius', 0);

    AppStorage.Set('enrollStatus', $r('app.string.enrolling'));
    this.startEnroll()
    Log.info(this.TAG, 'init-')
  }

  async startEnroll() {
    Log.info(this.TAG, 'startEnroll+')
    if (AppStorage.Get('xComponentSurfaceId') === "") {
      Log.info(this.TAG, 'surface id is not set, skip start enroll')
      return
    }
    if (this.enrolling) {
      Log.info(this.TAG, 'face is enrolling, skip start enroll')
      return
    }
    let setResult = await UserAuthModel.setSurfaceId(AppStorage.Get('xComponentSurfaceId'))
    Log.info(this.TAG, 'setSurfaceId result is ' + setResult)
    UserIDMModel.setOnAcquireCallback(this.onAcquire.bind(this))
    UserIDMModel.enrollFace().then(this.processResult.bind(this))
    this.enrolling = true
    Log.info(this.TAG, 'startEnroll-')
  }

  async processResult(result : number) {
    await this.faceDetected()
    await this.enrollProcessing()
    if (result == 0) {
      await this.enrollSuccess()
    } else {
      await this.enrollFail()
    }
    this.enrolling = false
  }

  async onAcquire(result: number) {
    Log.info(this.TAG, 'onAcquire+ result: ' + result)
    if (result === 25) {
      Log.info(this.TAG, 'onAcquire face detected+')
      this.faceDetected()
      Log.info(this.TAG, 'onAcquire face detected-')
    }
    Log.info(this.TAG, 'onAcquire-')
  }

  async faceDetected() {
    Log.info(this.TAG, 'faceDetected+')
    AppStorage.Set('animationTime', this.ANIMATION_TIME);
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Visible);
    AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Visible);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);

    AppStorage.Set('stackShelterHeight', AppStorage.Get('stackShelterHeightEnd'));
    await CommonController.sleepMS(this.ANIMATION_TIME)
    AppStorage.Set('animationTime', 0);
    Log.info(this.TAG, 'faceDetected-')
  }

  async enrollProcessing() {
    Log.info(this.TAG, 'enrollProcessing+')
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Visible);
    AppStorage.Set('stackProgressVisibility', Visibility.Visible);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Hidden);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);

    for (let i=0; i <= 100; i+=1) {
      await CommonController.sleepMS(30)
      AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
      AppStorage.Set('stackProgressValue', i);
      AppStorage.Set('stackProgressVisibility', Visibility.Visible);
    }

    Log.info(this.TAG, 'enrollProcessing-')
  }

  async enrollSuccess() {
    Log.info(this.TAG, 'enrollSuccess+')
    AppStorage.Set('stackVideoVisibility', Visibility.Hidden);
    AppStorage.Set('stackShelterVisibility', Visibility.Hidden);
    AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
    AppStorage.Set('stackSuccessVisibility', Visibility.Visible);
    AppStorage.Set('enrollTipVisibility', Visibility.Hidden);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);

    AppStorage.Set('enrollStatus', $r('app.string.enroll_success'));
    await CommonController.sleepMS(3000)
    router.replace({uri: 'pages/faceConfig'})
    Log.info(this.TAG, 'enrollSuccess-')
  }

  async enrollFail() {
    Log.info(this.TAG, 'enrollFail+')
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Visible);
    AppStorage.Set('stackProgressVisibility', Visibility.Visible);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Visible);
    AppStorage.Set('enrollButtonVisibility', Visibility.Visible);

    AppStorage.Set('enrollStatus', $r('app.string.enrolling_fail'));
    AppStorage.Set('enrollTip', $r('app.string.enroll_info_fail'))
    AppStorage.Set('enrollTipSize', Constants.ohos_id_text_size_body1);
    AppStorage.Set('stackVideoBlurRadius', 30);
    AppStorage.Set('stackProgressValue', 0);

    Log.info(this.TAG, 'enrollFail-')
  }

  async clear() {
    Log.info(this.TAG, 'clear+')
    await UserAuthModel.clearSurfaceId()
    Log.info(this.TAG, 'clear-')
  }
}

let enrollingController = new EnrollingController();
export default enrollingController as EnrollingController