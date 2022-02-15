// @ts-nocheck
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

import Constants from '../default/common/constant.ets';
import UserIDM from '@ohos.useridm';

var mPrgvalue;
var mResultCode = Constants.resultCode();
var mFaceTips = Constants.FaceTips();


export class model_3D {
  private UserIDM;

  constructor() {
    this.UserIDM = UserIDM.constructor();
  }
  /**
   * init FaceModel
   */
  initFaceModel(credentialInfo) {
    console.info("FaceEnroll model_3D initFaceModel start");
    AppStorage.Link('width');
    AppStorage.Link('height');
    AppStorage.Link('showPrg');
    mPrgvalue = AppStorage.Link('prgValue');
    AppStorage.Link('showBtn');
    AppStorage.Link('blur');
    AppStorage.Link('showBack');
    AppStorage.Set('enrollInfo', $r('app.string.enroll_info'))
    AppStorage.Set('enrollTitle', $r('app.string.face_record'))
    AppStorage.Link('showAperture1')
    this.startEnroll(credentialInfo);
    console.info("FaceEnroll model_3D initFaceModel end");
  }

   /**
   * resume page, unInit FaceModel
   */
  uninitFaceModel() {
    console.info(" FaceEnroll model_3D uninitFaceModel start");
    if (mPrgvalue != Constants.PROGRESS_TOTAL) {
      this.cancelEnroll();
    }
    setTimeout(() => {
      AppStorage.Set('width', Constants.SHELTER_START_WIDTH);
      AppStorage.Set('height', Constants.SHELTER_START_HEIGHT);
      AppStorage.Set('showPrg', false)
      mPrgvalue = AppStorage.Set('prgValue', $r('app.float.progress_value_start'))
      AppStorage.Set('showBack', true)
      AppStorage.Set('blur',  Constants.BLUR_START)
      AppStorage.Set('showBtn', false)
      AppStorage.Set('enrollInfo', $r('app.string.enroll_info'))
      AppStorage.Set('enrollTitle', $r('app.string.face_record'))
      AppStorage.Set('showAperture1',true)
      AppStorage.Set('showAperture2',false)
      AppStorage.Set('showAperture3',false)
      AppStorage.Set('showAperture4',false)
      AppStorage.Set('showAperture5',false)

    },Constants.TIMEOUT_500)
  }

  /**
   * Open session and get challenge.
   * @param callback
   */
  openSession(callback) {
    console.info("FaceEnroll model_3D openSession start");
    this.UserIDM.openSession((data) => {
      try {
        console.info("FaceEnroll model_3D openSession data", data);
        callback(data);
      }
      catch (e) {
        console.info("FaceEnroll model_3D openSession error", e);
      }
    })
    callback();
    console.info("FaceEnroll model_3D openSession end");
  }

  /**
   * start enroll, get callback value
   */
  startEnroll(credentialInfo) {
    console.info(" FaceEnroll model_3D startEnroll 3d start ");
    this.UserIDM.addCredential(credentialInfo, {
      onResult: (result, extraInfo) => {
        if (result == Constants.RESULT_CODE_SUCCESS) {
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
            AppStorage.Set('showPrg', true);
            Constants.progress_2D();
            this.endEnroll();
        }
        if (result == Constants.RESULT_CODE_FAIL) {
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
        }
        if (result == Constants.RESULT_CODE_TIMEOUT) {
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
          console.info(" FaceEnroll model_3D startEnroll UserIDM.addCredential onResult result", result);
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
    console.info("FaceEnroll model_3D endEnroll start");
    this.UserIDM.closeSession();
    console.info("FaceEnroll model_3D endEnroll userEnroll.closeSession()");
    AppStorage.Delete('faceSession');
    console.info("FaceEnroll model_3D endEnroll end");
  }

  /**
   * cancel enroll
   */
  cancelEnroll() {
    console.info("FaceEnroll model_3D cancelEnroll start");
    let can = this.UserIDM.cancel(AppStorage.Get('faceSession'));
    console.info("FaceEnroll model_2D cancelEnroll userEnroll.cancel()", can);
    this.endEnroll();
    console.info("FaceEnroll model_3D cancelEnroll end");
  }
}
let mFaceModel = new model_3D();
export default mFaceModel as model_3D;