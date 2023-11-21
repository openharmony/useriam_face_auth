/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
import BackComponent from '../component/backComponent'
import Log from '../utils/log'
import CommonController from '../controller/commonController'
import Constants from '../config/constant'

@Extend(Text) function introSetTitleFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_primary'))
  .fontSize($r('sys.float.ohos_id_text_size_headline7'))
  .fontWeight(FontWeight.Bold)
}

@Extend(Text) function introSetSubtitleFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_primary'))
  .fontSize($r('sys.float.ohos_id_text_size_sub_title1'))
  .fontWeight(FontWeight.Medium)
}

@Extend(Span) function introSetSpanFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_secondary'))
  .fontSize($r('sys.float.ohos_id_text_size_body2'))
  .fontWeight(FontWeight.Regular)
}

@Extend(Span) function introSetLinkFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_hyperlink'))
  .fontSize($r('sys.float.ohos_id_text_size_body2'))
  .fontWeight(FontWeight.Medium)
}

@Entry
@Component
struct EnrollIntro {
  @State readonly TAG: string = 'EnrollIntro'
  @StorageLink('enrollImageHeight') enrollImageHeight: number = 0
  @StorageLink('enrollImageWidth') enrollImageWidth: number = 0
  @StorageLink('CONTENT_TYPE_WIDTH') CONTENT_TYPE_WIDTH: number = 0
  @StorageLink('BUTTON_TYPE_WIDTH') BUTTON_TYPE_WIDTH: number = 0

  build() {
      Column() {
        BackComponent({prevTag:this.TAG})

        Column() {
          Column() {
            Scroll() {
              Column() {
                Text($r('app.string.face_checker'))
                  .introSetTitleFormat()
                  .margin({
                    top : '12vp',
                    bottom: '24vp'
                  })
                Image(CommonController.getStartViewImage())
                  .objectFit(ImageFit.Contain)
                  .width(this.enrollImageWidth)
                  .height(this.enrollImageHeight)
                  .margin({
                    bottom: '24vp'
                  })
                Column() {
                  Text($r('app.string.enroll_face_guide_title'))
                    .introSetSubtitleFormat()
                    .margin({
                      bottom: '8vp'
                    })
                  Text() {
                    Span($r('app.string.face_guide_2d_way'))
                    .introSetSpanFormat()
                    Span($r('app.string.face_guide_2d_announce'))
                    .introSetSpanFormat()
                    Span($r('app.string.face_guide_2d_announce_link'))
                    .introSetLinkFormat()
                    .onClick(() => {
                      router.push({ uri: 'pages/enrollDeclare' })
                    })
                    Span($r('app.string.period'))
                    .introSetSpanFormat()
                  }
                    .textAlign(TextAlign.Center)
                }
              }
            }
            .scrollBarColor(Constants.ohos_id_color_foreground_60_transparency)
            .scrollBarWidth('5vp')
            .margin({ right: '12vp' })
          }
            .width(this.CONTENT_TYPE_WIDTH)
            .flexGrow(1)

          Column() {
            Button({ type: ButtonType.Capsule, stateEffect: true }) {
              Text($r('app.string.face_recognition_guide'))
                .fontSize($r('sys.float.ohos_id_text_size_button1'))
                .fontColor($r('sys.color.ohos_id_color_foreground_contrary'))
                .fontWeight(FontWeight.Medium)
            }
            .backgroundColor(Constants.ohos_id_color_control_actived)
            .width(this.BUTTON_TYPE_WIDTH)
            .height('40vp')
            .onClick(() => {
              router.replace({ uri: 'pages/enrolling' })
            })
            .margin({
              top: '16vp',
              bottom: '24vp'
            })
          }
            .width(this.CONTENT_TYPE_WIDTH)
        }
        .width('100%')
        .height('100%')
        .flexShrink(1)
        .onAreaChange((_, newArea) => {
          Log.info(this.TAG, 'new non app bar area :' + JSON.stringify(newArea))
          CommonController.setNonAppBarDisplaySize(Number(newArea.width), Number(newArea.height))
          Log.info(this.TAG, 'non app bar area w: ' + newArea.width + ' h: ' + newArea.height)
        })
      }
      .width('100%')
      .height('100%')
      .backgroundColor($r('sys.color.ohos_id_color_sub_background'))
      .onAreaChange((_, newArea) => {
        Log.info(this.TAG, 'new display area :' + JSON.stringify(newArea))
        CommonController.setDisplaySize(Number(newArea.width), Number(newArea.height))
        Log.info(this.TAG, 'total area w: ' + newArea.width + ' h: ' + newArea.height)
      })
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}