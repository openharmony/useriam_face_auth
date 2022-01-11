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
import router from '@system.router';
import userEnroll from '@ohos.enroll'
import Constants from '../default/common/constant.ets'

const TAG = 'faceenroll-faceModel_3D';
var mPrgValue
var mValue = Constants.value()
var mPro = Constants.progress()
var mRes = Constants.result()


async function routeSuccess() {
  let options = {
    uri: 'pages/success'
  }
  try {
    await router.push(options)
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`)
  }
}

async function route_back() {
  let options = {
    uri: 'pages/index'
  }
  try {
    await router.push(options)
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`)
  }
}

export class model_3D {
  //    init page
  initFaceModel() {
    Log.showInfo(TAG, 'initFaceModel');

    AppStorage.Link('width');
    AppStorage.Link('height');
    AppStorage.Link('showPrg');
    mPrgValue = AppStorage.Link('prgValue');
    AppStorage.Link('showBtn');
    AppStorage.Link('blur');
    AppStorage.Link('showBack');
    AppStorage.Set('enrollInfo', $r('app.string.enroll_info'))
    AppStorage.Set('enrollTitle', $r('app.string.face_record'))
    AppStorage.Link('showAperture1')

    var result = userEnroll.init()
    Log.showInfo(TAG,'initFaceModel init result:' + result)

    var challenge = userEnroll.preEnroll()
    Log.showInfo(TAG,'initFaceModel perEnroll challenge:'+ challenge)

    var angle = userEnroll.getAngleDim()
    Log.showInfo(TAG,'initFaceModel getAngleDim angle:' + angle )

    this.startEnroll();
  }
  //resume page
  uninitFaceModel() {
    if(mPrgValue != Constants.PROGRESS_TOTAL)
    {
      var cal = userEnroll.cancelEnroll()
      Log.showInfo(TAG,'uninitFaceModel cancel cal:' + cal)
    }

    setTimeout(()=> {
      Log.showInfo(TAG, 'uninitFaceModel');

      AppStorage.Set('width', Constants.SHELTER_START_WIDTH);
      AppStorage.Set('height', Constants.SHELTER_START_HEIGHT);
      AppStorage.Set('showPrg', false)
      mPrgValue = AppStorage.Set('prgValue', $r('app.float.progress_value_start'))
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

  //get callback value
  private startEnroll(){
    userEnroll.enroll({enrollParam:{
      token: Constants.PARAM_TOKEN,
      flags: Constants.FLAGS,
      faceId: Constants.FACEID,
      previewId: Constants.PREVIEWID
    }},function(data) {
      Log.showInfo(TAG,'startEnroll enroll data:' + data.code)
      Log.showInfo(TAG,'startEnroll enroll data:' + data.errorCode)

      if(data.code == Constants.CODE_ACQUIRE) {
        if (data.errorCode == Constants.ERROR_CODE_FACE_DETECTED) {
          setTimeout(() => {
            AppStorage.Set('width', Constants.SHELTER_END_WIDTH);
            AppStorage.Set('height', Constants.SHELTER_3D_END_HEIGHT);
          }, Constants.TIMEOUT_1000)
          Constants.aperture()

        }

        var pro = mPro.has(data.errorCode)

        Log.showInfo(TAG,'startEnroll enroll pro:' + pro)
        if(pro) {
          AppStorage.Set('showPrg',true)
          mPrgValue = AppStorage.Set('prgValue',mPro.get(data.errorCode))
        }

        var have = mValue.has(data.errorCode)
        Log.showInfo(TAG,'startEnroll enroll have:' + have)
        if(have){
          AppStorage.Set('enrollInfo',mValue.get(data.errorCode))
        }
      }

      if(data.code == Constants.CODE_CANCEL) {
        Log.showInfo(TAG,'startEnroll cancel data:' + data.errorCode)
      }

      if(data.code == Constants.CODE_RESULT) {
        if(data.errorCode == Constants.ERROR_CODE_SUCCESS){

            routeSuccess()
            setTimeout(()=>{
              route_back()
            },Constants.TIMEOUT_1000)
        }

        var res = mRes.has(data.errorCode)
        if(res){
          AppStorage.Set('enrollTitle', $r('app.string.face_record_3d_record_fail'))
          AppStorage.Set('enrollInfo',mRes.get(data.errorCode))
          AppStorage.Set('blur',Constants.BLUR_END)
          AppStorage.Set('showBtn',true)
          AppStorage.Set('showBack',false)
          AppStorage.Set('prgValue',$r('app.float.progress_value_start'))
          AppStorage.Set('showAperture5',false)
        }
        if(data.errorCode == Constants.ERROR_CODE_TIMEOUT){
          AppStorage.Set('enrollTitle', $r('app.string.enrolling_timeout'))
          AppStorage.Set('enrollInfo','')
          AppStorage.Set('blur',Constants.BLUR_END)
          AppStorage.Set('showBtn',true)
          AppStorage.Set('showBack',false)
          AppStorage.Set('prgValue',$r('app.float.progress_value_start'))
          AppStorage.Set('showAperture5',false)
        }
      }
    })
    this.endEnroll()
  }
  //post and release
  private endEnroll(){
    var post = userEnroll.postEnroll()
    Log.showInfo(TAG,'endEnroll postEnroll post:' +post)

    var res = userEnroll.release()
    Log.showInfo(TAG,'endEnroll release res:' + res )
  }

  private toSuccess() {
    routeSuccess()

  }

}
let mFaceModel = new model_3D();
export default mFaceModel as model_3D;