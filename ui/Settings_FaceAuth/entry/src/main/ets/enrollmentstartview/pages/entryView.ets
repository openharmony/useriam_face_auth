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
import Constants from '../config/constant.ets'
import EntryViewController from '../controller/entryViewController.ets'
import CommonController from '../controller/commonController.ets'

@Entry
@Component
struct Index {
  private TAG: string = "Index:"
  @State resultString: string = ""

  build() {
    Column() {
      BackComponent({ prevTag: this.TAG })
      Text("临时认证界面")
        .fontSize(Constants.TITLE_FONT_SIZE)
        .fontWeight(FontWeight.Bold)
      Text("请将PIN码设置为123456后，点击认证按钮")
        .fontSize(Constants.FONT_SIZE)
      Button({ type: ButtonType.Capsule, stateEffect: true }){
        Text('认证').fontSize(Constants.BUTTON_FONT_SIZE).fontColor(0xffffff).fontSize(Constants.TITLE_FONT_SIZE)
      }
      .backgroundColor($r('app.color.startView_color'))
      .onClick(()=>{
        EntryViewController.doPINAuth()
          .catch(()=>{this.resultString = "认证失败，请将密码设置为123456后重试"})
      })
      .width(Constants.BUTTON_WIDTH)
      .height(Constants.BUTTON_HEIGHT)
      .margin({
        top: "24",
        bottom: "24"
      })
      Text(this.resultString)
        .fontSize(Constants.FONT_SIZE)
        .fontWeight(FontWeight.Bold)
    }.alignItems(HorizontalAlign.Center)
    .width("100%")
    .height("100%")
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}