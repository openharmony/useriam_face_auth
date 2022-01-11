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

import Constants from '../common/constant.ets'
import router from '@system.router';
@Entry
@Component

export default

struct Declare {
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center}) {
        Image($r('app.media.back'))
          .width(Constants.BACK_WIDTH)
          .height(Constants.BACK_HEIGHT)
          .margin({ top: $r('app.float.back_margin') })
          .onClick(() => {
            router.back()
          })
      }

      Text($r('app.string.face_recognize_annouce'))
        .fontSize(Constants.TITLE_FONT_SIZE)
        .fontWeight(FontWeight.Bold)
        .margin({ top: Constants.TITLE_MARGIN })

      Text() {
        Span($r('app.string.face_recognize_annouce_message_01'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_02'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_03'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_04'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_05'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_06'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_07'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_08'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_09'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_10'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_11'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_12'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
      }.margin({ top: Constants.CONTENT2_MARGIN })
      .textAlign(TextAlign.Start)
      .width(Constants.ENROLL_INFO_WIDTH)

      Text($r('app.string.face_recognize_annouce_message_13'))
        .fontSize(Constants.SMALL_TITLE_FONT_SIZE)
        .fontWeight(Constants.FONT_WEIGHT)
        .textAlign(TextAlign.Center)

      Text() {
        Span($r('app.string.face_recognize_annouce_message_14'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
        Span($r('app.string.face_recognize_annouce_message_15'))
          .textCase(TextCase.UpperCase)
          .fontSize(Constants.FONT_SIZE)
      }.margin({ top: Constants.CONTENT2_MARGIN })
      .textAlign(TextAlign.Start)
      .width(Constants.ENROLL_INFO_WIDTH)
    }.width('100%')
    .height('100%')
  }
}