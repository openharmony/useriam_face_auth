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
import CommonController from '../controller/commonController.ets'
import FaceConfigController from '../controller/faceConfigController.ets'
import ConfirmDialog from '../component/confirmDialog.ets'

@Entry
@Component
struct FaceConfig {
  @State TAG: string = "FaceConfig:"
  @State touched: boolean = false
  private deleteFaceDialogController: CustomDialogController = new CustomDialogController({
    builder: ConfirmDialog({
      title: $r('app.string.delete_record_title'),
      content: $r('app.string.wether_delete_face_data'),
      confirmText: $r('app.string.face_record_delete'),
      cancelText: $r('app.string.face_record_cancel'),
      confirmOperation: this.deleteFace.bind(this)
    }),
    alignment: CommonController.getDialogAlignment()
  })

  deleteFace() {
    Log.info(this.TAG, 'deleteFace+')
    FaceConfigController.deleteFace()
    Log.info(this.TAG, 'deleteFace-')
  }

  build() {
    Column() {
      BackComponent({ prevTag: this.TAG, title: $r('app.string.face_checker') })
      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems:ItemAlign.Center }) {
        Row() {
          Text($r('app.string.delete_record_title'))
            .fontSize(22)
            .fontColor(Color.Red)
            .margin({left:24, top:8, bottom:8, right:0})
            .textAlign(TextAlign.Start);
        }
      }
      .backgroundColor(this.touched? "#D8D8D8":"#FFFFFF")
      .height(56)
      .margin({left:24, right:24, top:16})
      .borderRadius(24)
      .onClick(()=>{
        this.deleteFaceDialogController.open()
      })
      .onTouch((event: TouchEvent) => {
        if (event.type === TouchType.Down) {
          this.touched = true;
        }
        if (event.type === TouchType.Up) {
          this.touched = false;
        }
      });
    }.width("100%")
    .height("100%")
    .backgroundColor("#E3E3E3");
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}