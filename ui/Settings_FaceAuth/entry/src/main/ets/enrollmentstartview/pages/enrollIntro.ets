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
import BackComponent from '../component/backComponent.ets'
import Log from '../utils/log.ets'
import CommonController from '../controller/commonController.ets'
import Constants from '../config/constant.ets'

@Entry
@Component
struct EnrollIntro {
  @State TAG: string = "EnrollIntro:"
  @StorageLink('enrollImageHeight') enrollImageHeight: number = 0
  @StorageLink('enrollImageWidth') enrollImageWidth: number = 0

  build() {
      Column() {
        BackComponent({prevTag:this.TAG})

        Column() {
          Column() {
            Scroll() {
              Column() {
                Text($r('app.string.face_checker'))
                  .fontSize(Constants.TITLE_FONT_SIZE)
                  .fontWeight(FontWeight.Bold)
                  .margin({
                    bottom: "20"
                  })
                Image($r('app.media.startImage_2D'))
                  .objectFit(ImageFit.Contain)
                  .width("1000%")
                  .height(this.enrollImageHeight)
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
                      .onClick(() => {
                        router.push({ uri: 'pages/enrollDeclare' })
                      })
                  }
                    .margin({ top: Constants.CONTENT2_MARGIN })
                    .textAlign(TextAlign.Center)
                }
                  .width(Constants.ENROLL_INFO_WIDTH)
                  .margin({
                    top: Constants.IMAGE_MARGIN,
                  })
              }
            }.scrollable(ScrollDirection.Vertical).scrollBarColor(Color.Gray).scrollBarWidth(30)
          }.width('100%').flexGrow(1)

          Flex({ justifyContent: FlexAlign.Center }) {
            Button({ type: ButtonType.Capsule, stateEffect: true }) {
              Text($r('app.string.face_recognitin_guide')).fontSize(Constants.BUTTON_FONT_SIZE).fontColor(0xffffff)
            }
            .backgroundColor($r('app.color.startView_color'))
            .width(Constants.BUTTON_WIDTH)
            .height(Constants.BUTTON_HEIGHT)
            .onClick(() => {
              router.replace({ uri: 'pages/enrolling' })
            })
            .margin({
              top: "24",
              bottom: "24"
            })
          }
        }.width("100%").height("100%")
        .constraintSize({
          maxWidth: '720'
        })
        .onAreaChange((_, newArea) => {
          Log.info(this.TAG, "new non app bar area :" + JSON.stringify(newArea))
          CommonController.setNonAppBarDisplaySize(Number(newArea.width), Number(newArea.height))
          Log.info(this.TAG, "non app ba aboutToAppear- w: " + newArea.width + " h: " + newArea.height)
        })
      }
      .width('100%')
      .height('100%')
      .onAreaChange((_, newArea) => {
        Log.info(this.TAG, "new display area :" + JSON.stringify(newArea))
        CommonController.setDisplaySize(Number(newArea.width), Number(newArea.height))
        Log.info(this.TAG, "aboutToAppear- w: " + newArea.width + " h: " + newArea.height)
      })
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}