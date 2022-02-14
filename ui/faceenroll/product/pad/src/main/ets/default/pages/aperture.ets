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

import Constants from '../common/constant.ets';

@Entry
@Component

export default

struct Aperture {

  @StorageLink('showAperture1') showAperture1:boolean = true;
  @StorageLink('showAperture2') showAperture2:boolean = false;
  @StorageLink('showAperture3') showAperture3:boolean = false;
  @StorageLink('showAperture4') showAperture4:boolean = false;
  @StorageLink('showAperture5') showAperture5:boolean = false;

  build() {
    Flex({ alignItems: ItemAlign.Center , justifyContent:FlexAlign.Center}) {
      if (this.showAperture1) {
        Image($r('app.media.aperture1'))
          .width(Constants.APERTURE1_WIDTH)
          .height(Constants.APERTURE1_WIDTH)
          .align(Alignment.Center)
      }
      if (this.showAperture2) {
        Image($r('app.media.aperture2'))
          .width(Constants.APERTURE2_WIDTH)
          .height(Constants.APERTURE2_WIDTH)
          .align(Alignment.Center)
      }
      if (this.showAperture3) {
        Image($r('app.media.aperture3'))
          .width(Constants.APERTURE34_WIDTH)
          .height(Constants.APERTURE34_WIDTH)
          .align(Alignment.Center)
      }
      if (this.showAperture4) {
        Image($r('app.media.aperture4'))
          .width(Constants.APERTURE34_WIDTH)
          .height(Constants.APERTURE34_WIDTH)
          .align(Alignment.Center)
      }
      if (this.showAperture5) {
        Image($r('app.media.aperture5'))
          .width(Constants.APERTURE5_WIDTH)
          .height(Constants.APERTURE5_WIDTH)
          .align(Alignment.Center)
      }
    }.position({x: $r('app.float.enroll_center_position_X'),y:Constants.ENROLL_CENTER_POSITION_Y})
    .width('100%')
    .height(Constants.CENTER_HEIGHT)
  }
}