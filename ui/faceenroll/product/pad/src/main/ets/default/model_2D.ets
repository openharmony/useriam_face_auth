// @ts-nocheck
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

import Constants from './common/constant.ets';
import UserIDM from '@ohos.useridm';

var mPrgvalue;
var mResultCode = Constants.resultCode();
var mFaceTips = Constants.FaceTips();

export class model_2D {
  private UserIDM;

  constructor() {
    this.UserIDM = UserIDM.constructor();
  }
  /**
   * init FaceModel
   */
  initFaceModel(credentialInfo) {
    console.info("FaceEnroll model_2D initFaceModel start");
    AppStorage.Link('width');
    AppStorage.Link('showBack');
    AppStorage.Link('height');
    AppStorage.Link('showAperture');
    AppStorage.Link('blur');
    AppStorage.Link('showPrg');
    mPrgvalue = AppStorage.Link('prgValue');
    AppStorage.Link('enrollInfo');
    AppStorage.Link('showBtn');
    AppStorage.Link('enrollTitle');
    this.startEnroll(credentialInfo);
    console.info("FaceEnroll model_2D initFaceModel end");
  }

  /**
   * Open session and get challenge.
   * @param callback
   */
  openSession(callback) {
    console.info("FaceEnroll model_2D openSession start");
    this.UserIDM.openSession((data) => {
      try {
        console.info("FaceEnroll model_2D openSession data", data);
        callback(data);
      }
      catch (e) {
        console.info("FaceEnroll model_2D openSession error", e);
      }
    })
    callback();
    console.info("FaceEnroll model_2D openSession end");
  }

  /**
   * resume page, unInit FaceModel
   */
  uninitFaceModel() {
    console.info(" FaceEnroll model_2D uninitFaceModel start");
    if (mPrgvalue != Constants.PROGRESS_TOTAL) {
      this.cancelEnroll();
    }
    setTimeout(() => {
      AppStorage.Set('width', Constants.SHELTER_START_WIDTH)
      AppStorage.Set('height', Constants.SHELTER_START_HEIGHT)
      AppStorage.Set('showBtn', false)
      AppStorage.Set('showPrg', false)
      AppStorage.Set('showBack', true)
      AppStorage.Set('blur', Constants.BLUR_START)
      AppStorage.Set('showAperture', true)
      mPrgvalue = AppStorage.Set('prgValue', $r('app.float.progress_value_start'))
      AppStorage.Set('enrollInfo', $r('app.string.enroll_info'))
      AppStorage.Set('enrollTitle', $r('app.string.face_record'))
    }, Constants.TIMEOUT_500)
    console.info(" FaceEnroll model_2D uninitFaceModel end");
  }

  /**
   * start enroll, get callback value
   */
  startEnroll(credentialInfo) {
    console.info(" FaceEnroll model_2D startEnroll 2d start ");
    this.UserIDM.addCredential(credentialInfo, {
      onResult: (result, extraInfo) => {
        if (result == Constants.RESULT_CODE_SUCCESS) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('showPrg', true);
          Constants.progress_2D();
          this.endEnroll();
        }
        if (result == Constants.RESULT_CODE_FAIL) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_GENERAL_ERROR) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_CANCELED) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
        }
        if (result == Constants.RESULT_CODE_TIMEOUT) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.enrolling_timeout'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_TYPE_NOT_SUPPORT) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_TRUST_LEVEL_NOT_SUPPORT) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_BUSY) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_INVALID_PARAMETERS) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_LOCKED) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
        if (result == Constants.RESULT_CODE_NOT_ENROLLED) {
          console.info(" FaceEnroll model_2D startEnroll UserIDM.addCredential onResult result", result);
          AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
          AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
          AppStorage.Set('showAperture', false);
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'));
          AppStorage.Set('enrollInfo', mResultCode.get(result));
          AppStorage.Set('blur', Constants.BLUR_END);
          AppStorage.Set('showBtn', true);
          AppStorage.Set('showBack', false);
          AppStorage.Set('prgValue', $r('app.float.progress_value_start'));
        }
      },
      onAcquireInfo: (callbackModule, acquire, extraInfo) => {
        AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
        AppStorage.Set('height', Constants.SHELTER_END_HEIGHT);
        AppStorage.Set('showAperture', false);
        AppStorage.Set('enrollTitle', $r('app.string.enrolling'));
        AppStorage.Set('enrollInfo', mFaceTips.get(acquire));
      }
    })
    this.endEnroll();
  }

  /**
   * end enroll
   */
  endEnroll() {
    console.info("FaceEnroll model_2D endEnroll start");
    this.UserIDM.closeSession();
    console.info("FaceEnroll model_2D endEnroll userEnroll.closeSession()");
    AppStorage.Delete('faceSession');
    console.info("FaceEnroll model_2D endEnroll end");
  }

  /**
   * cancel enroll
   */
  cancelEnroll() {
    console.info("FaceEnroll model_2D cancelEnroll start");
    let can = this.UserIDM.cancel(AppStorage.Get('faceSession'));
    console.info("FaceEnroll model_2D cancelEnroll userEnroll.cancel()", can);
    this.endEnroll();
    console.info("FaceEnroll model_2D cancelEnroll end");
  }

  /**
   * change index 3d
   */
  change_3D() {
    AppStorage.Set('show_startViews', '3D');
  }

  /**
   * change index 2d
   */
  change_2D() {
    AppStorage.Set('show_startViews', '2D');
  }

  /**
   * init index
   */
  init() {
    AppStorage.SetAndLink('show_startViews', '2D');
  }
}

let mFaceModel = new model_2D();

export default mFaceModel as model_2D;
