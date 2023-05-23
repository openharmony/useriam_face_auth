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

import Constants from '../config/constant'
import BackComponent from '../component/backComponent'
import Log from '../utils/log'
import CommonController from '../controller/commonController'

@Extend(Text) function setTitleFormat() {
  .fontColor(Constants.ohos_id_color_text_primary)
  .fontSize(Constants.ohos_id_text_size_body1)
  .fontWeight(FontWeight.Medium)
}

@Extend(Text) function setContentFormat() {
  .fontColor(Constants.ohos_id_color_text_primary)
  .fontSize(Constants.ohos_id_text_size_body1)
  .fontWeight(FontWeight.Regular)
}

@Extend(Span) function setSpanFormat() {
  .fontColor(Constants.ohos_id_color_text_primary)
  .fontSize(Constants.ohos_id_text_size_body1)
  .fontWeight(FontWeight.Regular)
}

@Entry
@Component
export default struct EnrollDeclare {
  private pattern: string = "2D";
  private readonly TAG: string = "EnrollDeclare"
  @StorageLink('CONTENT_TYPE_WIDTH') CONTENT_TYPE_WIDTH: number = 0

  build() {
    Column() {
      BackComponent({ prevTag: this.TAG, title: $r('app.string.face_guide_2d_announce_link') })

      Column() {
        Scroll() {
          Column() {
            Column() {
              Column() {
                Text($r("app.string.face_feature_desc"))
                  .setTitleFormat()
                  .margin({ bottom: '16vp' })
                Text($r("app.string.face_recognize_announce_message_01"))
                  .setContentFormat()
                  .margin({ bottom: '24vp' })
                Text($r("app.string.face_recognize_announce_message_02"))
                  .setTitleFormat()
                  .margin({ bottom: '16vp' })
                Text($r("app.string.face_recognize_announce_message_03"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_recognize_announce_message_04"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_recognize_announce_message_05"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_recognize_announce_message_06"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_recognize_announce_message_07"))
                  .setContentFormat()
                  .margin({ bottom: '24vp' })
                Text($r("app.string.face_recognize_announce_message_08"))
                  .setTitleFormat()
                  .margin({ bottom: '16vp' })
                Text($r("app.string.face_recognize_announce_message_09"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_announce_tip1_item2_3d"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_recognize_announce_message_10"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_recognize_announce_message_11"))
                  .setContentFormat()
                  .margin({ bottom: '8vp' })
                Text($r("app.string.face_recognize_announce_message_12"))
                  .setContentFormat()
                  .margin({ bottom: '24vp' })
                Text($r("app.string.face_recognize_announce_message_13"))
                  .setTitleFormat()
                  .margin({ bottom: '16vp' })
                Text() {
                  Span($r("app.string.face_recognize_announce_message_14_1"))
                    .setSpanFormat()
                  Span($r("app.string.face_recognize_announce_message_14_3"))
                    .setSpanFormat()
                    .fontWeight(FontWeight.Bold)
                  Span($r("app.string.face_recognize_announce_message_14_2"))
                    .setSpanFormat()
                }
              }
              .alignItems(HorizontalAlign.Start)
              .margin({ left: "24vp", right: '24vp' })
            }
            .width(this.CONTENT_TYPE_WIDTH)
          }
          .margin({ top: "24vp" })
          .width("100%")
        }
        .scrollBarColor(Constants.ohos_id_color_foreground_60_transparency)
        .scrollBarWidth('5vp')
        .margin({ right: "12vp", bottom: "24vp" })
      }
      .width("100%")
      .height('100%')
      .flexShrink(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}