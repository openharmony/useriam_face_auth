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
import EnrollingController from '../controller/enrollingController'

@Extend(Text) function enrollSetTitleFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_primary'))
  .fontSize($r('sys.float.ohos_id_text_size_headline7'))
  .fontWeight(FontWeight.Bold)
}

@Extend(Text) function enrollSetSubTitleFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_primary'))
  .fontSize($r('sys.float.ohos_id_text_size_sub_title1'))
  .fontWeight(FontWeight.Medium)
}

@Entry
@Component
struct Enrolling {
  private readonly TAG: string = 'Enrolling'
  private xComponentController = new XComponentController()
  @StorageLink('enrollStatus') enrollStatus: string = ''
  @StorageLink('stackVideoVisibility') stackVideoVisibility: Visibility = Visibility.Visible
  @StorageLink('stackVideoBlurRadius') stackVideoBlurRadius: number = 0
  @StorageLink('stackShelterVisibility') stackShelterVisibility: Visibility = Visibility.Visible
  @StorageLink('stackShelterHeight') stackShelterHeight : string = ''
  @StorageLink('stackSuccessVisibility') stackSuccessVisibility: Visibility = Visibility.Hidden
  @StorageLink('enrollTip') enrollTip: string = ''
  @StorageLink('enrollTipSize') enrollTipSize: string = ''
  @StorageLink('enrollTipVisibility') enrollTipVisibility: Visibility = Visibility.Hidden
  @StorageLink('enrollButtonVisibility') enrollButtonVisibility: Visibility = Visibility.Hidden
  @StorageLink('enrollImageHeight') enrollImageHeight: number = 0
  @StorageLink('enrollImageWidth') enrollImageWidth: number = 0
  @StorageLink('xComponentSurfaceId') xComponentSurfaceId : string = ''
  @StorageLink('BUTTON_TYPE_WIDTH') BUTTON_TYPE_WIDTH: number = 0
  @StorageLink('CONTENT_TYPE_WIDTH') CONTENT_TYPE_WIDTH: number = 0
  // Ring
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  @StorageLink('stackRingRadius') stackRingRadius: number = 0
  @StorageLink('stackProgressValue') stackProgressValue : number = 0
  @StorageLink('stackProgressVisibility') stackProgressVisibility: Visibility = Visibility.Hidden
  @StorageLink('animationTime') animationTime: number = 0

  aboutToAppear() {
    Log.info(this.TAG, 'aboutToAppear+')
    EnrollingController.init()
    Log.info(this.TAG, 'aboutToAppear-')
  }

  onPageHide() {
    Log.info(this.TAG, 'onPageHide+')
    EnrollingController.clear()
    Log.info(this.TAG, 'onPageHide-')
  }

  getCanvaRadian(progress : number) : number {
    return -0.5 * Math.PI + progress * 2 * Math.PI / 100
  }

  build() {
      Column()  {
        BackComponent({prevTag:this.TAG})
        Text(this.enrollStatus)
          .enrollSetTitleFormat()
          .margin({
            top: '12vp',
            bottom: '24vp'
          })
        Stack() {
          XComponent({
            id: 'faceEnrollPreview',
            type: 'surface',
            controller: this.xComponentController
          })
            .visibility(this.stackVideoVisibility)
            .height('100%')
            .width('100%')
            .markAnchor({ x: '50%', y: '50%'})
            .position({ x: '50%', y: '50%'})
            .onLoad(()=>{
              Log.info(this.TAG, 'onLoad+')
              this.xComponentSurfaceId = this.xComponentController.getXComponentSurfaceId()
              EnrollingController.startEnroll()
              Log.info(this.TAG, 'onLoad-')
            })
          Image($r('app.media.shelter'))
            .objectFit(ImageFit.Contain)
            .height(this.stackShelterHeight)
            .width('1000%')
            .clip(true)
            .markAnchor({ x: '50%', y: '50%' })
            .position({ x: '50%', y: '50%' })
            .animation({ duration: this.animationTime, curve: Curve.Linear })
            .visibility(this.stackShelterVisibility)
          Canvas(this.context)
            .width('100%')
            .height('100%')
            .backgroundColor('#00000000')
            .markAnchor({ x: '50%', y: '50%' })
            .position({ x: '50%', y: '50%' })
            .visibility(this.stackProgressVisibility)
            .onReady(() =>{
              this.context.imageSmoothingEnabled = true
              this.context.clearRect(0, 0, this.enrollImageWidth, this.enrollImageHeight)
              this.context.beginPath()
              this.context.lineWidth = 4
              this.context.lineCap = 'round'
              this.context.strokeStyle = Constants.ohosIdColorForeground97Transparency
              this.context.arc(this.enrollImageWidth / 2, this.enrollImageHeight / 2, this.stackRingRadius,
                this.getCanvaRadian(0), this.getCanvaRadian(100), false);
              this.context.stroke()
              this.context.beginPath()
              this.context.lineWidth = 5
              this.context.lineCap = 'round'
              this.context.strokeStyle = Constants.ohosIdColorEmphasize
              this.context.arc(this.enrollImageWidth / 2, this.enrollImageHeight / 2, this.stackRingRadius,
                this.getCanvaRadian(0), this.getCanvaRadian(this.stackProgressValue), false);
              this.context.stroke()
            })
          Image($r('app.media.success'))
            .objectFit(ImageFit.Contain)
            .height('84vp')
            .width('100%')
            .visibility(this.stackSuccessVisibility)
        }
        .height(this.enrollImageHeight)
        .width(this.enrollImageWidth)
        .clip(true)
        Text(this.enrollTip)
        .enrollSetSubTitleFormat()
        .fontWeight(FontWeight.Medium)
        .textAlign(TextAlign.Center)
        .visibility(this.enrollTipVisibility)
        .width(this.CONTENT_TYPE_WIDTH)
        .margin({
          top: '24vp'
        })
        Blank()
        Button({ type: ButtonType.Capsule, stateEffect: true }) {
          Text($r('app.string.enroll_button_content'))
            .fontSize($r('sys.float.ohos_id_text_size_button1'))
            .fontWeight(FontWeight.Medium)
            .fontColor($r('sys.color.ohos_id_color_text_primary_activated'))
        }
          .backgroundColor($r('sys.color.ohos_id_color_button_normal'))
          .width(this.BUTTON_TYPE_WIDTH)
          .height('40vp')
          .visibility(this.enrollButtonVisibility)
          .onClick(() => {
            EnrollingController.init()
          })
          .margin({
            bottom: '24vp',
          })
      }
      .width('100%')
      .height('100%')
      .backgroundColor($r('sys.color.ohos_id_color_sub_background'))
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+')
    CommonController.routeBack()
    Log.info(this.TAG, 'onBackPress-')
    return true
  }
}