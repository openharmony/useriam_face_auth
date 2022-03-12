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

import router from '@system.router';
import mFaceModel from "../model_3D.ets";
import Constants from '../common/constant.ets';
const SUBTYPE_FACE_2D = 20000;
const AUTHTYPE_FACE = 2;

async function route_back() {
  let options = {
    uri: 'pages/entryView'
  }
  try {
    await router.push(options);
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`);
  }
}

@Entry
@Component
export default struct Enroll_3D {
  @StorageLink('showAperture') showAperture: boolean = true;
  @StorageLink('blur') blur: number = Constants.BLUR_START;
  @StorageLink('width') width: any = Constants.SHELTER_START_WIDTH;
  @StorageLink('height') height: any = Constants.SHELTER_START_HEIGHT;
  @StorageLink('showPrg') showPrg: boolean = false;
  @StorageLink('prgValue') prgValue: number = Constants.PROGRESS_START;
  @StorageLink('enrollInfo') enrollInfo: any = $r('app.string.enroll_info');
  @StorageLink('enrollTitle') enrollTitle: any = $r('app.string.face_record');
  @StorageLink('showBtn') showBtn: boolean = false;
  @StorageLink('showBack') showBack: boolean = true;
  private credentialInfo;

  private aboutToAppear(): void  {
    setTimeout(()=> {
      AppStorage.Delete('faceToken');
      router.push({
        uri: 'pages/entryView'
      });
    }, 180000)
    this.credentialInfo = {
      credType:AUTHTYPE_FACE,
      credSubType:SUBTYPE_FACE_2D,
      token:AppStorage.Get('token')
    };
    mFaceModel.initFaceModel(this.credentialInfo);
  }

  private onPageHide() {
    mFaceModel.uninitFaceModel();
  }

  private onBackPress(): void{
    AppStorage.Delete('faceToken');
    router.push({
      uri: 'pages/entryView',
    })
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Image($r('app.media.image'))
        .width('100%')
        .height(Constants.CENTER_HEIGHT)
        .align(Alignment.Center)
        .position({x: $r('app.float.enroll_center_position_X'),y:Constants.ENROLL_CENTER_POSITION_Y})

      Flex()
        .width('100%')
        .height(Constants.CENTER_HEIGHT)
        .align(Alignment.Center)
        .blur(this.blur)
        .position({x:$r('app.float.enroll_center_position_X'),y: Constants.ENROLL_CENTER_POSITION_Y})
        .animation({ duration: Constants.SHELTER_ANIMATOR, curve: Curve.Linear })

      if (this.showAperture) {
        Flex({ alignItems: ItemAlign.Center , justifyContent:FlexAlign.Center}) {
          Image($r('app.media.aperture1'))
            .width(Constants.APERTURE1_WIDTH)
            .height(Constants.APERTURE1_WIDTH)
            .align(Alignment.Center)
        }
        .position({x:$r('app.float.enroll_center_position_X'),y:Constants.ENROLL_CENTER_POSITION_Y})
        .height(Constants.CENTER_HEIGHT)
        .width('100%')
      }

      Flex()
        .width('100%')
        .height(Constants.ENROLL_CENTER_POSITION_Y)

      Flex({ alignItems: ItemAlign.Center , justifyContent:FlexAlign.Center}) {
        Image($r('app.media.shelter'))
          .width(this.width)
          .height(this.height)
          .animation({ duration: Constants.SHELTER_ANIMATOR, curve: Curve.Linear })
      }.width('100%')
      .height(Constants.CENTER_HEIGHT)


      if(this.showPrg) {
        Flex({ alignItems: ItemAlign.Center , justifyContent:FlexAlign.Center}) {
          Progress({ value: Constants.PROGRESS_START, total: Constants.PROGRESS_TOTAL, style: ProgressStyle.Capsule })
            .value(this.prgValue)
            .width(Constants.PROGRESS_WIDTH)
            .color(Color.Red)
        }.position({x:$r('app.float.enroll_center_position_X'),y:Constants.ENROLL_CENTER_POSITION_Y})
        .height(Constants.CENTER_HEIGHT)
        .width('100%')
      }

      if(this.showBack) {
        Flex() {
          Image($r('app.media.back'))
            .width(Constants.BACK_WIDTH)
            .height(Constants.BACK_HEIGHT)
            .onClick(() => {
              router.back()
            })
        }.position({ x: $r('app.float.enroll_center_position_X'), y: Constants.CENTER_POSITION_Y })
      }

      Flex({ alignItems: ItemAlign.Center , justifyContent:FlexAlign.Center}) {
        Text(this.enrollTitle)
          .fontSize(Constants.TITLE_FONT_SIZE)
          .fontWeight(Constants.ENROll_TITLE)
      }.position({x:$r('app.float.enroll_title_position_X'),y:Constants.TITLE_ENROLLING_MARGIN})

      Flex({justifyContent:FlexAlign.Center}) {
        Text(this.enrollInfo)
          .fontSize(Constants.ERROR_CODE_INFO_SIZE)
          .textAlign(TextAlign.Center)
      }
      .position({ x:$r('app.float.enroll_info_X'), y: Constants.ENROLL_INFO_POSITION})
      .width(Constants.ENROLL_INFO_WIDTH)

      if(this.showBtn){
        Flex({justifyContent:FlexAlign.Center}) {
          Button({ type: ButtonType.Capsule, stateEffect: true }) {
            Text($r('app.string.enroll_button_content'))
              .fontSize(Constants.BUTTON_FONT_SIZE)
              .fontColor($r('app.color.startView_color'))
          }
          .backgroundColor($r('app.color.newBtn_color'))
          .width(Constants.BUTTON_WIDTH)
          .height(Constants.BUTTON_HEIGHT)
          .onClick(() => {
            route_back()
          })
        }.position({ x:$r('app.float.enroll_center_position_X'), y: Constants.BUTTON_POSITION})
      }
    }
    .width('100%')
    .height('100%')
  }
}