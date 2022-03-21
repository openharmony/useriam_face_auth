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

import Constants from '../config/constant.ets'
import BackComponent from '../component/backComponent.ets'
import Log from '../utils/log.ets'
import CommonController from '../controller/commonController.ets'

@Entry
@Component
export default struct EnrollDeclare {
  private pattern: string = "2D";
  private TAG: string = "EnrollDeclare:"

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      BackComponent({ prevTag:this.TAG, title: $r('app.string.face_guide_2d_announce_link' )})
      Scroll() {
        Column() {
          Column() {
            Text($r("app.string.face_feature_desc"))
              .fontSize(Constants.DECLARE_SECONDARY_TITLE_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start)
              .margin({ top: Constants.DECLARE_TITLE_MARGIN_TOP, bottom: Constants.DECLARE_TITLE_MARGIN_BOTTOM })
            Text(this.pattern == "2D" ? $r("app.string.face_recognize_annouce_message_01") : $r("app.string.face_3drecognize_annouce_message_01"))
              .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start)
              .margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM})
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            Text($r("app.string.face_recognize_annouce_message_02"))
              .fontSize(Constants.DECLARE_SECONDARY_TITLE_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start)
              .margin({ top: Constants.DECLARE_TITLE_MARGIN_TOP, bottom: Constants.DECLARE_TITLE_MARGIN_BOTTOM })
            Text($r("app.string.face_recognize_annouce_message_03"))
              .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            Text($r("app.string.face_recognize_annouce_message_04"))
              .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            Text($r("app.string.face_recognize_annouce_message_05"))
              .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
            Text($r("app.string.face_recognize_annouce_message_06"))
              .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            Text($r("app.string.face_recognize_annouce_message_07"))
              .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            Text($r("app.string.face_recognize_annouce_message_08"))
              .fontSize(Constants.DECLARE_SECONDARY_TITLE_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ top: Constants.DECLARE_TITLE_MARGIN_TOP, bottom: Constants.DECLARE_TITLE_MARGIN_BOTTOM })

            Text($r("app.string.face_recognize_annouce_message_09"))
              .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            if (this.pattern == "2D") {
              Text($r("app.string.face_annouce_tip1_item2_3d"))
                .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                .width(Constants.DECLARE_MAX_WIDTH)
                .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
                .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
              Text() {
                Span(Constants.DECLARE_TITLE_3).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                Span($r("app.string.face_recognize_annouce_message_10")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              }
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)

              Text() {
                Span(Constants.DECLARE_TITLE_4).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                Span($r("app.string.face_recognize_annouce_message_11")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              }
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)

              Text() {
                Span(Constants.DECLARE_TITLE_5).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                Span($r("app.string.face_recognize_annouce_message_12")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              }
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            } else if (this.pattern == "3D") {
              Text() {
                Span(Constants.DECLARE_TITLE_2).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                Span($r("app.string.face_recognize_annouce_message_10")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              }
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)

              Text() {
                Span(Constants.DECLARE_TITLE_3).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                Span($r("app.string.face_recognize_annouce_message_11")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              }
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)

              Text() {
                Span(Constants.DECLARE_TITLE_4).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                Span($r("app.string.face_recognize_annouce_message_12")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              }
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start).margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
              .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
            }


            Text($r("app.string.face_recognize_annouce_message_13"))
              .fontSize(Constants.DECLARE_SECONDARY_TITLE_FONT_SIZE)
              .width(Constants.DECLARE_MAX_WIDTH)
              .textAlign(TextAlign.Start)
              .margin({ top: Constants.DECLARE_TITLE_MARGIN_TOP, bottom: Constants.DECLARE_TITLE_MARGIN_BOTTOM })


            Text() {
              Span($r("app.string.face_recognize_annouce_message_14_1"))
                .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              Span($r("app.string.face_recognize_annouce_message_14_3"))
                .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                .fontWeight(FontWeight.Bold)
                .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              Span($r("app.string.face_recognize_annouce_message_14_2"))
                .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              Span($r("app.string.face_recognize_annouce_message_14_3"))
                .fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                .fontWeight(FontWeight.Bold)

            }
            .width(Constants.DECLARE_MAX_WIDTH)
            .textAlign(TextAlign.Start)
            .margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
            .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)

            Text() {
              Span($r("app.string.face_recognize_annouce_message_15_1")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
              Span($r("app.string.face_recognize_annouce_message_14_3")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
                .fontWeight(FontWeight.Bold)
              Span($r("app.string.face_recognize_annouce_message_15_2")).fontSize(Constants.DECLARE_TEXT_FONT_SIZE)
            }
            .width(Constants.DECLARE_MAX_WIDTH)
            .textAlign(TextAlign.Start)
            .margin({ bottom: Constants.DECLARE_ITEM_MARGIN_BOTTOM })
            .lineHeight(Constants.DECLARE_TEXT_LINE_HEIGHT)
          }.width(Constants.DECLARE_WIDTH)

        }
      }
      .scrollBarWidth(Constants.DECLARE_SCROLL_BAR_WIDTH)
    }
    .backgroundColor(Constants.DECLARE_BACKGROUND_COLOR)
    .width(Constants.DECLARE_MAX_WIDTH)
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}