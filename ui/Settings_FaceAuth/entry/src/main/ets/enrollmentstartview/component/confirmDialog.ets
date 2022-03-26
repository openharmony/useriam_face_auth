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

import Log from '../utils/log.ets'
import DefaultConstants from '../config/defaultConstant.ets'

@CustomDialog
export default struct ConfirmDialog {
  private TAG: string = "ConfirmDialog:"
  private controller: CustomDialogController;
  @Prop title: string
  @Prop content: string
  @Prop confirmText: string
  @Prop cancelText: string
  private confirmOperation: ()=>{}

  build() {
    Column() {
      Text(this.title)
        .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_20 })
      Text(this.content)
        .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_16 })
        .margin({ left: -DefaultConstants.PADDING_10, bottom: DefaultConstants.PADDING_20 })

      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Text(this.cancelText)
          .fontColor($r('app.color.startView_color'))
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            Log.info(this.TAG, "click cancel+")
            this.controller.close()
            Log.info(this.TAG, "click cancel-")
          })
        Divider()
          .vertical(true)
          .strokeWidth(DefaultConstants.DIVIDER)
          .color(DefaultConstants.DARK_GREY_BACKGROUND_COLOR)
          .height(DefaultConstants.PADDING_30)
        Text(this.confirmText)
          .fontColor(Color.Red)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            Log.info(this.TAG, "click confirm+")
            this.confirmOperation()
            Log.info(this.TAG, "click confirm-")
            this.controller.close()
          })
      }
    }
    .width(DefaultConstants.TOTAL_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      bottom: DefaultConstants.PADDING_30,
      top: DefaultConstants.PADDING_20,
      left: DefaultConstants.PADDING_30,
      right: DefaultConstants.PADDING_30
    })
  }
}