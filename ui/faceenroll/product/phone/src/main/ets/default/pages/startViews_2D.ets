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
import Constants from '../common/constant.ets';
import mFaceModel from "../model_2D.ets";

async function routeDeclare() {
  let options = {
    uri: 'pages/declare'
  }
  try {
    await router.push(options);
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`);
  }
}
async function routeEnroll_2D() {
  let options = {
    uri: 'pages/enroll_2D'
  }
  try {
    await router.push(options);
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`);
  }
}

@Entry
@Component
export default
struct StartViews_2D {
  @StorageLink('checkbox_color') checkbox_color : any = $r('app.color.startView_color');
  private pageHideFlag: number = 0;

  public openSessionCallBack() {
    routeEnroll_2D();
  }

  private onPageHide(): void {
    if (this.pageHideFlag == 0) {
      router.push({
        uri: 'pages/entryView'
      })
    }
  }
  private onBackPress(): void{
    AppStorage.Delete('faceToken');
    router.push({
      uri: 'pages/entryView',
    })
  }
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {

      Flex() {
        Image($r('app.media.back'))
          .width(Constants.BACK_WIDTH)
          .height(Constants.BACK_HEIGHT)
          .onClick(()=>{
          })
      }.margin({ top: $r('app.float.back_margin') })
      .width('100%')


      Text($r('app.string.face_checker'))
        .fontSize(Constants.TITLE_FONT_SIZE)
        .fontWeight(FontWeight.Bold)
        .margin({ top: Constants.TITLE_MARGIN })

      Image($r('app.media.startImage_2D'))
        .width('100%')
        .height(Constants.IMAGE_2D_HEIGHT)
        .margin({ top: Constants.IMAGE_MARGIN})

      Column() {
        Text($r('app.string.enroll_face_guide_title'))
          .fontSize(Constants.SMALL_TITLE_FONT_SIZE)
          .fontWeight(Constants.FONT_WEIGHT)
          .textAlign(TextAlign.Center)

        Text($r('app.string.face_guide_2d_way'))
          .fontSize(Constants.FONT_SIZE)
          .fontColor($r('app.color.font_color'))
          .textAlign(TextAlign.Center)
          .margin({ top: Constants.CONTENT1_MARGIN })

        Text() {
          Span($r('app.string.face_guide_2d_announce'))
            .fontColor($r('app.color.font_color'))
            .textCase(TextCase.UpperCase)
            .fontSize(Constants.FONT_SIZE)
          Span($r('app.string.face_guide_2d_announce_link'))
            .textCase(TextCase.UpperCase)
            .fontSize(Constants.FONT_SIZE)
            .fontColor($r('app.color.startView_color'))
        }.margin({ top: Constants.CONTENT2_MARGIN })
        .textAlign(TextAlign.Center)
        .onClick(() => {
          routeDeclare();
        })

      }.width(Constants.ENROLL_INFO_WIDTH)
      .margin({ top: Constants.IMAGE_MARGIN })

      Flex() {
        Toggle({ type: ToggleType.Checkbox, isOn: true })
          .size({ width: Constants.CHECKBOX_SIZE, height: Constants.CHECKBOX_SIZE })
          .selectedColor(this.checkbox_color)
          .onChange((isOn: boolean) => {console.info('Component status:' + isOn)
          })
      }.position({x:Constants.CHECKBOX_POSITION_X,y:Constants.CHECKBOX_POSITION_Y})

      Text($r('app.string.face_2dguide_light_notify_universal'))
        .fontSize(Constants.FONT_SIZE)
        .fontColor(Color.Black)
        .position({x:Constants.CHECKBOX_TEXT_POSITION_X,y:Constants.CHECKBOX_TEXT_POSITION_Y})

      Flex({justifyContent:FlexAlign.Center}) {
        Button({ type: ButtonType.Capsule, stateEffect: true }) {
          Text($r('app.string.face_record')).fontSize(Constants.BUTTON_FONT_SIZE).fontColor(0xffffff)
        }
        .backgroundColor($r('app.color.startView_color')).width(Constants.BUTTON_WIDTH).height(Constants.BUTTON_HEIGHT)
        .onClick(() => {
          this.pageHideFlag = 1;
          mFaceModel.openSession(this.openSessionCallBack.bind(this));
        })
      }.position({ x:$r('app.float.enroll_center_position_X'), y: Constants.BUTTON_POSITION})
    }
    .width('100%')
    .height('100%')
  }
}