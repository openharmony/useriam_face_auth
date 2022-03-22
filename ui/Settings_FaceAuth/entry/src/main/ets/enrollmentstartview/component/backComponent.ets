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
import commonControler from '../controller/commonController.ets'

@Preview
@Component
export default struct BackComponent {
  private prevTag: string
  private title: string = ""
  private TAG : string = "BackComponent"
  @State isTouch: boolean = false

  aboutToAppear() {
    this.TAG = this.prevTag + "BackComp:"
  }

  build() {
    Row() {
      Image($r('app.media.back_op'))
        .height('50')
        .width('50')
        .borderRadius(15)
        .backgroundColor(this.isTouch? "#E3E3E3": "#00000000")
        .onClick(() => {
          Log.info(this.TAG, 'back image onClick +')
          commonControler.routeBack()
          Log.info(this.TAG, 'back image onClick -')
        })
        .margin({
          left: 15,
          top: 5,
          bottom: 5
        })
        .onTouch((event: TouchEvent) => {
          if (event.type === TouchType.Down) {
            this.isTouch = true;
          }
          if (event.type === TouchType.Up) {
            this.isTouch = false;
          }
        })
      Text(this.title)
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .width('90%')
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .margin({ left: 15, right: 15 });
    }.width('100%')
    .height(70)
    .width("100%")
    .alignItems(VerticalAlign.Center)
    .align(Alignment.Start);
  }
}