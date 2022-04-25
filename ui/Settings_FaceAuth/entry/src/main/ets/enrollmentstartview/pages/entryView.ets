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

import BackComponent from '../component/backComponent.ets'
import Log from '../utils/log.ets'
import EntryViewController from '../controller/entryViewController.ets'
import CommonController from '../controller/commonController.ets'
import Constants from '../config/constant.ets'

@Entry
@Component
struct Index {
  private TAG: string = "Index:"
  @State resultString: string = ""

  build() {
    Column() {
      BackComponent({ prevTag: this.TAG })
      Text("临时认证界面")
        .fontColor(Constants.ohos_id_color_text_primary)
        .fontSize(Constants.ohos_id_text_size_headline7)
        .fontWeight(FontWeight.Bold)
      Text("请将PIN码设置为123456后，点击认证按钮")
        .fontColor(Constants.ohos_id_color_text_primary)
        .fontSize(Constants.ohos_id_text_size_subtitle1)
        .fontWeight(FontWeight.Medium)
        .margin({
          top: "12",
          bottom: "24"
        })
      Button({ type: ButtonType.Capsule, stateEffect: true }){
        Text('认证')
          .fontColor(Constants.ohos_id_color_foreground_inverse)
          .fontSize(Constants.ohos_id_text_button1)
          .fontWeight(FontWeight.Medium)
      }
      .backgroundColor(Constants.ohos_id_color_control_actived)
      .onClick(()=>{
        EntryViewController.doPINAuth()
          .catch(()=>{this.resultString = "认证失败，请将密码设置为123456后重试"})
      })
      .width('30%')
      Text(this.resultString)
      .fontColor(Constants.ohos_id_color_text_primary)
      .fontSize(Constants.ohos_id_text_size_subtitle1)
      .fontWeight(FontWeight.Medium)
    }
    .alignItems(HorizontalAlign.Center)
    .width("100%")
    .height("100%")
    .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
    .onAreaChange((_, newArea) => {
      Log.info(this.TAG, "new display area :" + JSON.stringify(newArea))
      CommonController.setDisplaySize(Number(newArea.width), Number(newArea.height))
      Log.info(this.TAG, "total area w: " + newArea.width + " h: " + newArea.height)
    })
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}