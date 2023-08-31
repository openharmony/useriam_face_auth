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
import Log from '../utils/log'
import commonController from '../controller/commonController'

@Preview
@Component
export default struct BackComponent {
  private prevTag: string = ''
  private title: string | Resource = ''
  private tag : string = 'BackComponent'
  @State isTouch: boolean = false
  @StorageLink('CONTENT_TYPE_WIDTH') CONTENT_TYPE_WIDTH: number = 0

  aboutToAppear() {
    this.tag = this.prevTag + 'BackComp:'
  }

  build() {
    Row() {
      Image($r('app.media.back_op'))
        .height('24vp')
        .width('24vp')
        .backgroundColor(this.isTouch ? Constants.ohos_id_color_click_effect : '#00000000')
        .onClick(() => {
          Log.info(this.tag, 'back image onClick +')
          commonController.routeBack()
          Log.info(this.tag, 'back image onClick -')
        })
        .margin({
          top: '16vp',
          bottom: '16vp',
          left: '24vp',
          right: '16vp',
        })
        .responseRegion({ x: '-12vp', y: '-16vp', width: '48vp', height: '56vp' })
        .onTouch((event?: TouchEvent) => {
          Log.info(this.tag, 'touch event')
          if (event !== undefined) {
            if (event.type === TouchType.Down) {
              this.isTouch = true;
            }
            if (event.type === TouchType.Up) {
              this.isTouch = false;
            }
          }
        })
      Text(this.title)
        .fontColor(Constants.ohos_id_color_titlebar_text)
        .fontSize(Constants.ohos_id_text_size_headline8)
        .fontWeight(FontWeight.Bold)
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .margin({ right: '16vp' })
    }
    .height('56vp')
    .width(this.CONTENT_TYPE_WIDTH)
    .alignItems(VerticalAlign.Center)
    .align(Alignment.Start);
  }
}