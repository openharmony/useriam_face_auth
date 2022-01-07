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

import Log from '../../../../../../common/src/main/ets/default/Log.ets'
import userEnroll from '@ohos.enroll'
import Constants from './common/constant.ets'

const TAG = 'faceenroll-faceModel_2D'

var mPrgvalue
var mRes = Constants.result()

export class model_2D {
  //    init page
  initFaceModel() {
    Log.showInfo(TAG, 'initFaceModel');
    AppStorage.Link('width')
    AppStorage.Link('showBack')
    AppStorage.Link('height')
    AppStorage.Link('showAperture')
    AppStorage.Link('blur')
    AppStorage.Link('showPrg')
    mPrgvalue = AppStorage.Link('prgValue')
    AppStorage.Link('enrollInfo')
    AppStorage.Link('showBtn')
    AppStorage.Link('enrollTitle')

    var result = userEnroll.init()
    Log.showInfo(TAG,'initFaceModel init result:' + result)

    var challenge = userEnroll.preEnroll()
    Log.showInfo(TAG,'initFaceModel perEnroll challenge:'+ challenge)

    this.startEnroll()

  }
//resume page
  uninitFaceModel() {
    if(mPrgvalue != Constants.PROGRESS_TOTAL)
    {
      var cal = userEnroll.cancelEnroll()
      Log.showInfo(TAG,'uninitFaceModel cancel cal:' + cal)
    }
    setTimeout(()=> {
      Log.showInfo(TAG, 'uninitFaceModel');
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
    },Constants.TIMEOUT_500)
  }
//get callback value
  startEnroll(){
    userEnroll.enroll({
      enrollParam:{
        token: Constants.PARAM_TOKEN,
        flags: Constants.FLAGS,
        faceId: Constants.FACEID,
        previewId: Constants.PREVIEWID
      }
      },function(data) {
      Log.showInfo(TAG,'startEnroll enroll data:' + data.code)
      Log.showInfo(TAG,'startEnroll enroll data:' + data.errorCode)

      if(data.code == Constants.CODE_ACQUIRE) {
        if (data.errorCode == Constants.ERROR_CODE_FACE_DETECTED) {
          setTimeout(() => {
            AppStorage.Set('width', Constants.SHELTER_END_WIDTH)
            AppStorage.Set('height', Constants.SHELTER_END_HEIGHT)
            AppStorage.Set('showAperture', false)
          }, Constants.TIMEOUT_900)
        }
      }
      if(data.code == Constants.CODE_CANCEL) {
        Log.showInfo(TAG,'startEnroll cancel data:' + data.errorCode)
      }
      if(data.code == Constants.CODE_RESULT) {
        Log.showInfo(TAG,'enter result')
        if(data.errorCode == Constants.ERROR_CODE_SUCCESS){
          AppStorage.Set('showPrg',true)
          Constants.progress_2D()
          this.endEnroll()
        }
        var res = mRes.has(data.errorCode)
        if(res){
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'))
          AppStorage.Set('enrollInfo',mRes.get(data.errorCode))
          AppStorage.Set('blur',Constants.BLUR_END)
          AppStorage.Set('showBtn',true)
          AppStorage.Set('showBack',false)
          AppStorage.Set('prgValue',$r('app.float.progress_value_start'))
        }
        if(data.errorCode == Constants.ERROR_CODE_TIMEOUT){
          AppStorage.Set('enrollTitle', $r('app.string.enrolling_timeout'))
          AppStorage.Set('enrollInfo','')
          AppStorage.Set('blur',Constants.BLUR_END)
          AppStorage.Set('showBtn',true)
          AppStorage.Set('showBack',false)
          AppStorage.Set('prgValue',$r('app.float.progress_value_start'))
        }
      }
    })
    this.endEnroll()
  }
//post and release
  private endEnroll(){
    var post = userEnroll.postEnroll()
    Log.showInfo(TAG,'endEnroll postEnroll post:' + post)

    var res = userEnroll.release()
    Log.showInfo(TAG,'endEnroll release res:' + res)
  }

  change_3D() {
    AppStorage.Set('show_startViews','3D')
  }

  change_2D() {
    AppStorage.Set('show_startViews','2D')
  }
//init index
  init(){
    AppStorage.SetAndLink('show_startViews','2D')
  }
}
let mFaceModel = new model_2D();
export default mFaceModel as model_2D;
