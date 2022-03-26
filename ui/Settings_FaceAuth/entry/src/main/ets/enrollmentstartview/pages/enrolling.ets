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
import Constants from '../config/constant.ets'
import EnrollingController from '../controller/enrollingController.ets'

@Entry
@Component
struct Enrolling {
  private TAG: string = "Enrolling:"
  @StorageLink('enrollStatus') enrollStatus: string = ""
  @StorageLink('stackVideoVisibility') stackVideoVisibility: Visibility = Visibility.Visible
  @StorageLink('stackVideoBlurRadius') stackVideoBlurRadius: number = 0
  @StorageLink('stackShelterVisibility') stackShelterVisibility: Visibility = Visibility.Visible
  @StorageLink('stackShelterHeight') stackShelterHeight : string = ""
  @StorageLink('stackProgressVisibility') stackProgressVisibility: Visibility = Visibility.Hidden
  @StorageLink('stackProgressValue') stackProgressValue: number = 0
  @StorageLink('stackSuccessVisibility') stackSuccessVisibility: Visibility = Visibility.Hidden
  @StorageLink('enrollTip') enrollTip: string = ""
  @StorageLink('enrollTipVisibility') enrollTipVisibility: Visibility = Visibility.Hidden
  @StorageLink('enrollButtonVisibility') enrollButtonVisibility: Visibility = Visibility.Hidden
  @StorageLink('enrollImageHeight') enrollImageHeight: number = 0
  @StorageLink('enrollImageWidth') enrollImageWidth: number = 0
  @StorageLink('stackRingHeightPercentage') stackRingHeightPercentage: string = ""

  aboutToAppear() {
    Log.info(this.TAG, "aboutToAppear+")
    EnrollingController.init()
    Log.info(this.TAG, "aboutToAppear-")
  }

  build() {
      Column()  {
        BackComponent({prevTag:this.TAG})
        Text(this.enrollStatus)
          .fontSize(Constants.TITLE_FONT_SIZE)
          .fontWeight(FontWeight.Bold)
          .margin({
            bottom: "20"
          })
        Stack() {
          // to be replace by XComp
          Rect()
            .visibility(this.stackVideoVisibility)
            .blur(this.stackVideoBlurRadius)
            .height("500%")
            .width("500%")
          Image($r('app.media.shelter'))
            .objectFit(ImageFit.Contain)
            .height(this.stackShelterHeight)
            .width('1000%')
            .clip(true)
            .markAnchor({ x: '50%', y: '50%' })
            .position({ x: '50%', y: '50%' })
            .animation({ duration: 2000, curve: Curve.Linear })
            .visibility(this.stackShelterVisibility)
          Progress({ value: Constants.PROGRESS_START, total: Constants.PROGRESS_TOTAL, style: ProgressStyle.Ring })
            .value(this.stackProgressValue)
            .height(this.stackRingHeightPercentage)
            .color($r('app.color.startView_color'))
            .visibility(this.stackProgressVisibility)
            .markAnchor({ x: '50%', y: '50%' })
            .position({ x: '50%', y: '50%' })
          Image($r('app.media.success'))
            .objectFit(ImageFit.Contain)
            .height('30%')
            .width('100%')
            .visibility(this.stackSuccessVisibility)
        }
          .height(this.enrollImageHeight)
          .width(this.enrollImageWidth)
          .clip(true)
        Blank()
        Text(this.enrollTip)
          .fontSize(Constants.ERROR_CODE_INFO_SIZE)
          .textAlign(TextAlign.Center)
          .visibility(this.enrollTipVisibility)
          .constraintSize({
            maxWidth:720
          })
        Blank()
        Button({ type: ButtonType.Capsule, stateEffect: true }) {
          Text($r('app.string.enroll_button_content'))
            .fontSize(Constants.BUTTON_FONT_SIZE)
            .fontColor($r('app.color.startView_color'))
        }
          .backgroundColor($r('app.color.newBtn_color'))
          .width(Constants.BUTTON_WIDTH)
          .height(Constants.BUTTON_HEIGHT)
          .visibility(this.enrollButtonVisibility)
          .onClick(() => {
            EnrollingController.init()
          })
          .margin({
            top: "24",
            bottom: "24",
          })
      }
        .width('100%')
        .height('100%')
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}