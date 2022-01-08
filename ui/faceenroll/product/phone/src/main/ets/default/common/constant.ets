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
import router from '@system.router';
import Log from '../../../../../../../common/src/main/ets/default/Log.ets'

const TAG = 'constant'

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

export default class Constants {

//code and errorCode
  static CODE_RESULT = 1
  static CODE_CANCEL = 2
  static CODE_ACQUIRE = 3
  static ERROR_CODE_SUCCESS = 0
  static ERROR_CODE_FAIL = 1
  static ERROR_CODE_CANCEL = 2
  static ERROR_CODE_TIMEOUT = 4
  static ERROR_CODE_ALGORITHM_NOT_INIT = 5
  static ERROR_CODE_HAL_INVALID = 6
  static ERROR_CODE_OVER_MAX_FACES = 7
  static ERROR_CODE_INVALID_PARAMETERS = 9
  static ERROR_CODE_BUSY = 13
  static ERROR_CODE_ENROLL_HAS_REGISTERED = 16
  static ERROR_CODE_FACE_DETECTED = 38
  static ERROR_CODE_BAD_QUALITY = 4
  static ERROR_CODE_NOT_FOUND = 5
  static ERROR_CODE_TOO_SMALL = 6
  static ERROR_CODE_TOO_LARGE = 7
  static ERROR_CODE_OFFSET_LEFT = 8
  static ERROR_CODE_OFFSET_TOP = 9
  static ERROR_CODE_OFFSET_RIGHT = 10
  static ERROR_CODE_OFFSET_BOTTOM = 11
  static ERROR_CODE_LIVENESS_WARNING = 13
  static ERROR_CODE_LIVENESS_FAILURE = 14
  static ERROR_CODE_ROTATED_LEFT = 15
  static ERROR_CODE_RISE = 16
  static ERROR_CODE_ROTATED_RIGHT = 17
  static ERROR_CODE_DOWN = 18
  static ERROR_CODE_EYE_OCCLUSION = 21
  static ERROR_CODE_EYE_CLOSE = 22
  static ERROR_CODE_MOUTH_OCCLUSION = 23
  static ERROR_CODE_MULTI = 27
  static ERROR_CODE_BLUR = 28
  static ERROR_CODE_NOT_COMPLETE = 29
  static ERROR_CODE_DARKLIGHT = 30
  static ERROR_CODE_HIGHTLIGHT = 31
  static ERROR_CODE_HALF_SHADOW = 32
  static ERROR_CODE_HAS_REGISTERED = 37
  static ERROR_CODE_DARKPIC = 39
  static ERROR_CODE_ROTATE_TOP_RIGHT = 40
  static ERROR_CODE_ROTATE_TOP_LEFT = 41
  static ERROR_CODE_ROTATE_BOTTOM_RIGHT = 42
  static ERROR_CODE_ROTATE_BOTTOM_LEFT = 43
  static ERROR_CODE_OPEN_CAMERA_FAILED = 46
  static ERROR_CODE_PROGRESS_FIRST = 1004
  static ERROR_CODE_PROGRESS_SECOND = 1008
  static ERROR_CODE_PROGRESS_THIRD = 1012
  static PARAM_TOKEN = "check"
  static FLAGS = 0
  static FACEID = 0
  static PREVIEWID = 2

  //setTimeout
  static TIMEOUT_100 = 100
  static TIMEOUT_200 = 200
  static TIMEOUT_300 = 300
  static TIMEOUT_400 = 400
  static TIMEOUT_500 = 500
  static TIMEOUT_600 = 600
  static TIMEOUT_700 = 700
  static TIMEOUT_800 = 800
  static TIMEOUT_900 = 900
  static TIMEOUT_1000 = 1000
  static TIMEOUT_1100 = 1100
  static TIMEOUT_1200 = 1200
  static TIMEOUT_1300 = 1300
  static TIMEOUT_1400 = 1400
  static TIMEOUT_1500 = 1500
  static TIMEOUT_1600 = 1600
  static TIMEOUT_1700 = 1700
  static TIMEOUT_1800 = 1800
  static TIMEOUT_1900 = 1900
  static TIMEOUT_2000 = 2000
  static TIMEOUT_3000 = 3000

  //UI
  static SHELTER_ANIMATOR = 1000
  static PROGRESS_START = 0
  static PROGRESS_TOTAL = 100
  static FONT_WEIGHT = 500
  static ENROll_TITLE = 600
  static SHELTER_START_WIDTH = 1100
  static SHELTER_END_WIDTH = '100%'
  static CENTER_HEIGHT = '620lpx'
  static SHELTER_START_HEIGHT = '2000lpx'
  static SHELTER_END_HEIGHT = '640lpx'
  static SHELTER_3D_END_HEIGHT = '640lpx'
  static PROGRESS_VALUE_FIRST = 30
  static PROGRESS_VALUE_SECOND = 60
  static PROGRESS_VALUE_THIRD = 100
  static SPACE = 20
  static INITIAL_INDEX = 0
  static HEIGHT_LIST = 60
  static FONTSIZE_16 = 16
  static BORDER_RADIUS = 10
  static STROKE_WIDTH = 2
  static START_MARGIN = 20
  static END_MARGIN = 20
  static SHOW_START_VIEWS_3D = '3D'
  static SHOW_START_VIEWS_2D = '2D'
  static LIST_PADDING = 5
  static BLUR_START = 0
  static BLUR_END = 40
  static CENTER_POSITION_Y = 5
  static BUTTON_POSITION = '92%'
  static ENROLL_INFO_POSITION = '918lpx'
  static ENROLL_INFO_WIDTH = '90%'
  static CHECKBOX_POSITION_X = '5%'
  static CHECKBOX_POSITION_Y = '85%'
  static CHECKBOX_TEXT_POSITION_X = '18%'
  static CHECKBOX_TEXT_POSITION_Y = '87%'

  static PROGRESS_WIDTH = '460lpx'
  static BACK_WIDTH = '136lpx'
  static BACK_HEIGHT = '110lpx'
  static TITLE_FONT_SIZE = '48lpx'
  static TITLE_MARGIN = '24lpx'
  static TITLE_ENROLLING_MARGIN = '140lpx'
  static IMAGE_MARGIN  = '96lpx'
  static IMAGE_3D_MARGIN = '48lpx'
  static FONT_SIZE = '28lpx'
  static SMALL_TITLE_FONT_SIZE = '36lpx'
  static CONTENT1_MARGIN = '16lpx'
  static CONTENT2_MARGIN = '8lpx'
  static BUTTON_FONT_SIZE = '32lpx'
  static BUTTON_WIDTH = '384lpx'
  static BUTTON_HEIGHT = '80lpx'
  static IMAGE_2D_HEIGHT = '480lpx'
  static IMAGE_3D_WIDTH = '512lpx'
  static ERROR_CODE_INFO_SIZE = '32lpx'
  static ENROLL_CENTER_POSITION_Y = '240lpx'
  static SUCCESS_WIDTH = '204lpx'
  static CHECKBOX_SIZE = '70lpx'
  static APERTURE1_WIDTH = '266lpx'
  static APERTURE2_WIDTH = '264lpx'
  static APERTURE34_WIDTH = '284lpx'
  static APERTURE5_WIDTH = '466lpx'

 //PROGRESS
  static PROGRESS_VALUE_5 = 5
  static PROGRESS_VALUE_10 = 10
  static PROGRESS_VALUE_15 = 15
  static PROGRESS_VALUE_20 = 20
  static PROGRESS_VALUE_25 = 25
  static PROGRESS_VALUE_30 = 30
  static PROGRESS_VALUE_35 = 35
  static PROGRESS_VALUE_40 = 40
  static PROGRESS_VALUE_45 = 45
  static PROGRESS_VALUE_50 = 50
  static PROGRESS_VALUE_55 = 55
  static PROGRESS_VALUE_60 = 60
  static PROGRESS_VALUE_65 = 65
  static PROGRESS_VALUE_70 = 70
  static PROGRESS_VALUE_75 = 75
  static PROGRESS_VALUE_80 = 80
  static PROGRESS_VALUE_85 = 85
  static PROGRESS_VALUE_90 = 90
  static PROGRESS_VALUE_95 = 95
  static PROGRESS_VALUE_100 = 100

//errorCode value
   static value(){
     const info = new Map()
     info.set(Constants.ERROR_CODE_BAD_QUALITY,$r('app.string.enroll_info_bad_quality'))
     info.set(Constants.ERROR_CODE_NOT_FOUND,$r('app.string.enroll_info_not_found'))
     info.set(Constants.ERROR_CODE_TOO_SMALL,$r('app.string.face_record_3d_tip_close'))
     info.set(Constants.ERROR_CODE_TOO_LARGE,$r('app.string.face_record_3d_tip_far'))
     info.set(Constants.ERROR_CODE_OFFSET_LEFT,$r('app.string.face_enroll_too_left'))
     info.set(Constants.ERROR_CODE_OFFSET_TOP,$r('app.string.face_enroll_too_high'))
     info.set(Constants.ERROR_CODE_OFFSET_RIGHT,$r('app.string.face_enroll_too_right'))
     info.set(Constants.ERROR_CODE_OFFSET_BOTTOM,$r('app.string.face_enroll_too_low'))
     info.set(Constants.ERROR_CODE_LIVENESS_WARNING,$r('app.string.enroll_info_liveness_warning'))
     info.set(Constants.ERROR_CODE_LIVENESS_FAILURE,$r('app.string.enroll_info_liveness_failure'))
     info.set(Constants.ERROR_CODE_ROTATED_LEFT,$r('app.string.face_rotate_too_left'))
     info.set(Constants.ERROR_CODE_RISE,$r('app.string.face_rotate_too_high'))
     info.set(Constants.ERROR_CODE_ROTATED_RIGHT,$r('app.string.face_rotate_too_right'))
     info.set(Constants.ERROR_CODE_DOWN,$r('app.string.face_rotate_too_low'))
     info.set(Constants.ERROR_CODE_EYE_OCCLUSION,$r('app.string.enroll_info_eye_occlusion'))
     info.set(Constants.ERROR_CODE_EYE_CLOSE,$r('app.string.enroll_info_eye_close'))
     info.set(Constants.ERROR_CODE_MOUTH_OCCLUSION,$r('app.string.enroll_info_mouth_occlusion'))
     info.set(Constants.ERROR_CODE_MULTI,$r('app.string.face_record_3d_tip_multi'))
     info.set(Constants.ERROR_CODE_BLUR,$r('app.string.enroll_info_blur'))
     info.set(Constants.ERROR_CODE_NOT_COMPLETE,$r('app.string.enroll_info_not_complete'))
     info.set(Constants.ERROR_CODE_DARKLIGHT,$r('app.string.enroll_info_darklight'))
     info.set(Constants.ERROR_CODE_HIGHTLIGHT,$r('app.string.enroll_info_highlight'))
     info.set(Constants.ERROR_CODE_HALF_SHADOW,$r('app.string.enroll_info_half_shadow'))
     info.set(Constants.ERROR_CODE_HAS_REGISTERED,$r('app.string.enroll_info_face_has_registered'))
     info.set(Constants.ERROR_CODE_DARKPIC,$r('app.string.enroll_info_darkpic'))
     info.set(Constants.ERROR_CODE_ROTATE_TOP_RIGHT,$r('app.string.enroll_info_rotate_top_right'))
     info.set(Constants.ERROR_CODE_ROTATE_TOP_LEFT,$r('app.string.enroll_info_rotate_top_left'))
     info.set(Constants.ERROR_CODE_ROTATE_BOTTOM_RIGHT,$r('app.string.enroll_info_rotate_bottom_right'))
     info.set(Constants.ERROR_CODE_ROTATE_BOTTOM_LEFT,$r('app.string.enroll_info_rotate_bottom_left'))
     info.set(Constants.ERROR_CODE_OPEN_CAMERA_FAILED,$r('app.string.enroll_info_open_camera_failed'))

     return info
   }
  //face progress_3D
  static progress() {
    const pro = new Map()
    pro.set(Constants.ERROR_CODE_PROGRESS_FIRST,Constants.PROGRESS_VALUE_FIRST)
    pro.set(Constants.ERROR_CODE_PROGRESS_SECOND,Constants.PROGRESS_VALUE_SECOND)
    pro.set(Constants.ERROR_CODE_PROGRESS_THIRD,Constants.PROGRESS_VALUE_THIRD)

    return pro
  }

  static result() {
    const res = new Map()
    res.set(Constants.ERROR_CODE_FAIL,'')
    res.set(Constants.ERROR_CODE_CANCEL,$r('app.string.enroll_info_cancel'))
    res.set(Constants.ERROR_CODE_ALGORITHM_NOT_INIT,$r('app.string.enroll_info_algorithm_not_init'))
    res.set(Constants.ERROR_CODE_HAL_INVALID,$r('app.string.enroll_info_hal_invalid'))
    res.set(Constants.ERROR_CODE_OVER_MAX_FACES,$r('app.string.enroll_info_over_max_faces'))
    res.set(Constants.ERROR_CODE_INVALID_PARAMETERS,$r('app.string.enroll_info_invalid_parameters'))
    res.set(Constants.ERROR_CODE_BUSY,$r('app.string.enroll_info_busy'))
    res.set(Constants.ERROR_CODE_ENROLL_HAS_REGISTERED,$r('app.string.enroll_info_has_registered'))

    return res
  }

  //face progress_2D
  static progress_2D(){
    Log.showInfo(TAG,'progress_2D')

    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_5)
    },Constants.TIMEOUT_100);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_10)
    },Constants.TIMEOUT_200);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_15)
    },Constants.TIMEOUT_300);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_20)
    },Constants.TIMEOUT_400);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_25)
    },Constants.TIMEOUT_500);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_30)
    },Constants.TIMEOUT_600);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_35)
    },Constants.TIMEOUT_700);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_40)
    },Constants.TIMEOUT_800);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_45)
    },Constants.TIMEOUT_900);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_50)
    },Constants.TIMEOUT_1000);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_55)
    },Constants.TIMEOUT_1100);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_60)
    },Constants.TIMEOUT_1200);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_65)
    },Constants.TIMEOUT_1300);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_70)
    },Constants.TIMEOUT_1400);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_75)
    },Constants.TIMEOUT_1500);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_80)
    },Constants.TIMEOUT_1600);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_85)
    },Constants.TIMEOUT_1700);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_90)
    },Constants.TIMEOUT_1800);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_95)
    },Constants.TIMEOUT_1900);
    setTimeout(()=>{
      AppStorage.Set('prgValue',Constants.PROGRESS_VALUE_100)
      routeSuccess()
    },Constants.TIMEOUT_2000);
    setTimeout(()=>{
      route_back()
    },Constants.TIMEOUT_3000)
  }

  static aperture(){
    setTimeout(() => {
      AppStorage.Set('showAperture1',false)
      AppStorage.Set('showAperture2',true)
    }, Constants.TIMEOUT_1000)
    setTimeout(() => {
      AppStorage.Set('showAperture2',false)
      AppStorage.Set('showAperture3',true)
    }, Constants.TIMEOUT_1300)
    setTimeout(() => {
      AppStorage.Set('showAperture3',false)
      AppStorage.Set('showAperture4',true)
    }, Constants.TIMEOUT_1600)
    setTimeout(() => {
      AppStorage.Set('showAperture4',false)
      AppStorage.Set('showAperture5',true)
    }, Constants.TIMEOUT_1900)
  }
}