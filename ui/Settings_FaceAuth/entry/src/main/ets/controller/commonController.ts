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

import Log from '../utils/log';
import router from '@system.router';
import Config from '../config/config';
import UserIdmModel from '../model/userIdmModel';

class CommonController {
  private readonly TAG: string = 'CommonController';

  async routeBack(): Promise<void> {
    Log.info(this.TAG, 'router getLength: ' + router.getLength());
    if (parseInt(router.getLength()) > 1) {
      Log.info(this.TAG, 'router back: back+');
      router.back();
      Log.info(this.TAG, 'router back: back-');
    } else {
      Log.info(this.TAG, 'router back: terminate ability+');
      this.terminateAbility();
      Log.info(this.TAG, 'router back: terminate ability-');
    }
  }

  async terminateAbility(): Promise<void> {
    Log.info(this.TAG, 'terminate ability+');
    UserIdmModel.destroy();
    globalThis.abilityContext.terminateSelf()
      .then(data => Log.info(this.TAG, 'terminateSelf promise then ' + data))
      .catch(error => Log.error(this.TAG, 'terminateSelf promise catch ' + error));
    Log.info(this.TAG, 'terminate ability-');
  }

  async sleepMS(ms): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

  getDialogAlignment():DialogAlignment {
    if (Config.getDeviceType() === 'phone') {
      return DialogAlignment.Bottom;
    } else {
      return DialogAlignment.Center;
    }
  }

  getDialogYOffset():string {
    if (Config.getDeviceType() === 'phone') {
      return '-12vp';
    } else {
      return '0vp';
    }
  }

  getShelterHeightPercentageBegin(width: number, height: number) :string {
    const HALF = 0.5;
    let a = width * HALF;
    let b = height * HALF;
    let R = Math.sqrt(a * a + b * b);
    const R_H_RATIO = 0.3;
    let H = R / R_H_RATIO;
    const H_WEIGHT = 166;
    let hPercentage = '' + Math.trunc(H * H_WEIGHT / height) + '%';
    return hPercentage;
  }

  setNonAppBarDisplaySize(nonAppBarDisplayWidth: number, nonAppBarDisplayHeight: number): void {
    const HALF = 0.5;
    if (Config.getDeviceType() !== 'phone') {
      return;
    }
    nonAppBarDisplayHeight = nonAppBarDisplayHeight + AppStorage.Get<number>('SYSTEM_NAVIGATION_BAR_HEIGHT');
    Log.info(this.TAG, 'update nonAppBarDisplayHeight to ' + nonAppBarDisplayHeight);
    let sideLength = Math.floor(nonAppBarDisplayHeight * HALF);
    AppStorage.SetOrCreate('enrollImageHeight', sideLength);
    AppStorage.SetOrCreate('enrollImageWidth', nonAppBarDisplayWidth);
    // when percentage is 166%, circular diameter == height
    AppStorage.SetOrCreate('stackShelterHeightBegin',
      this.getShelterHeightPercentageBegin(nonAppBarDisplayWidth, nonAppBarDisplayHeight * HALF));
    AppStorage.SetOrCreate('stackShelterHeightEnd', '132.8%'); // 166 * 0.8
    const STACK_SHELTER_HEIGHT_END_RATIO = 1.328;
    const SCALE = 0.6;
    const STACKRINGRADIUS_OFFSET = 21;
    AppStorage.SetOrCreate('stackRingRadius', (sideLength * STACK_SHELTER_HEIGHT_END_RATIO * SCALE +
      STACKRINGRADIUS_OFFSET) * HALF);
  }

  setDisplaySize(displayWidth: number, displayHeight: number): void {
    this.setGridWidth(displayWidth);
    if (Config.getDeviceType() !== 'tablet') {
      return;
    }
    displayHeight = displayHeight + AppStorage.Get<number>('SYSTEM_NAVIGATION_BAR_HEIGHT') +
      AppStorage.Get<number>('SYSTEM_STATUS_BAR_HEIGHT');
    Log.info(this.TAG, 'update displayHeight to ' + displayHeight);
    const HALF = 0.5;
    const DISPLAY_SIDE_LENGTH_RATIO = 0.8;
    let sideLength = Math.floor(Math.min(displayWidth, displayHeight * HALF) * DISPLAY_SIDE_LENGTH_RATIO * HALF) / HALF;
    Log.info(this.TAG, 'sideLength ' + sideLength);
    AppStorage.SetOrCreate('enrollImageHeight', sideLength);
    AppStorage.SetOrCreate('enrollImageWidth', sideLength);
    // when percentage is 166%, circular diameter == height
    AppStorage.SetOrCreate('stackShelterHeightBegin',
      this.getShelterHeightPercentageBegin(sideLength, sideLength));
    AppStorage.SetOrCreate('stackShelterHeightEnd', '146%'); // 166 * 0.88
    const STACK_SHELTER_HEIGHT_END_RATIO = 1.46;
    const SCALE = 0.6;
    const STACKRINGRADIUS_OFFSET = 21;
    AppStorage.SetOrCreate('stackRingRadius', Math.floor((sideLength * STACK_SHELTER_HEIGHT_END_RATIO * SCALE +
      STACKRINGRADIUS_OFFSET) * HALF));
  }

  setGridWidth(displayWidth: number): void {
    if (Config.getDeviceType() === 'tablet') {
      this.setGridWidthForTablet(displayWidth);
      return;
    }
    this.setGridWidthForPhone(displayWidth);
  }

  setGridWidthForTablet(displayWidth: number): void {
    const COLUMN_NUM = 12;
    const MARGIN = 24;
    const GUTTER_WIDTH = 24;
    const MARGIN_WEIGHT = 2;
    let columnWidth = (displayWidth - MARGIN * MARGIN_WEIGHT - (COLUMN_NUM - 1) * GUTTER_WIDTH) / COLUMN_NUM;
    const CONTENT_COLUMN_WIDTH_WEIGHT = 8;
    const CONTENT_GUTTER_WIDTH_WEIGHT = 7;
    AppStorage.SetOrCreate('CONTENT_TYPE_WIDTH', CONTENT_COLUMN_WIDTH_WEIGHT * columnWidth + CONTENT_GUTTER_WIDTH_WEIGHT * GUTTER_WIDTH);
    const BUTTON_COLUMN_WIDTH_WEIGHT = 4;
    const BUTTON_GUTTER_WIDTH_WEIGHT = 3;
    AppStorage.SetOrCreate('BUTTON_TYPE_WIDTH', BUTTON_COLUMN_WIDTH_WEIGHT * columnWidth + BUTTON_GUTTER_WIDTH_WEIGHT * GUTTER_WIDTH);
    const POP_COLUMN_WIDTH_WEIGHT = 6;
    const POP_GUTTER_WIDTH_WEIGHT = 5;
    AppStorage.SetOrCreate('POP_TYPE_WIDTH', POP_COLUMN_WIDTH_WEIGHT * columnWidth + POP_GUTTER_WIDTH_WEIGHT * GUTTER_WIDTH);
    const CARD_MARGIN = 12;
    const CARD_GUTTER_WIDTH = 12;
    const CARD_COLUMN_WIDTH_WEIGHT = 8;
    const CARD_GUTTER_WIDTH_WEIGHT = 7;
    columnWidth = (displayWidth - CARD_MARGIN * MARGIN_WEIGHT - (COLUMN_NUM - 1) * CARD_GUTTER_WIDTH) / COLUMN_NUM;
    AppStorage.SetOrCreate('CARD_TYPE_WIDTH', CARD_COLUMN_WIDTH_WEIGHT * columnWidth + CARD_GUTTER_WIDTH_WEIGHT * CARD_GUTTER_WIDTH);
  }

  setGridWidthForPhone(displayWidth: number): void {
    const COLUMN_NUM = 4;
    const MARGIN = 24;
    const GUTTER_WIDTH = 24;
    const MARGIN_WEIGHT = 2;
    let columnWidth = (displayWidth - MARGIN * MARGIN_WEIGHT - (COLUMN_NUM - 1) * GUTTER_WIDTH) / COLUMN_NUM;
    const CONTENT_COLUMN_WIDTH_WEIGHT = 4;
    const CONTENT_GUTTER_WIDTH_WEIGHT = 3;
    AppStorage.SetOrCreate('CONTENT_TYPE_WIDTH', CONTENT_COLUMN_WIDTH_WEIGHT * columnWidth + CONTENT_GUTTER_WIDTH_WEIGHT * GUTTER_WIDTH);
    const BUTTON_COLUMN_WIDTH_WEIGHT = 2;
    const BUTTON_GUTTER_WIDTH_WEIGHT = 3;
    AppStorage.SetOrCreate('BUTTON_TYPE_WIDTH', BUTTON_COLUMN_WIDTH_WEIGHT * columnWidth + BUTTON_GUTTER_WIDTH_WEIGHT * GUTTER_WIDTH);
    const POP_COLUMN_WIDTH_WEIGHT = 4;
    const POP_GUTTER_WIDTH_WEIGHT = 3;
    AppStorage.SetOrCreate('POP_TYPE_WIDTH', POP_COLUMN_WIDTH_WEIGHT * columnWidth + POP_GUTTER_WIDTH_WEIGHT * GUTTER_WIDTH);
    const CARD_MARGIN = 12;
    const CARD_GUTTER_WIDTH = 12;
    const CARD_COLUMN_WIDTH_WEIGHT = 4;
    const CARD_GUTTER_WIDTH_WEIGHT = 3;
    columnWidth = (displayWidth - CARD_MARGIN * MARGIN_WEIGHT - (COLUMN_NUM - 1) * CARD_GUTTER_WIDTH) / COLUMN_NUM;
    AppStorage.SetOrCreate('CARD_TYPE_WIDTH', CARD_COLUMN_WIDTH_WEIGHT * columnWidth + CARD_GUTTER_WIDTH_WEIGHT * CARD_GUTTER_WIDTH);
  }

  uin8Array2JsonString(inArray: Uint8Array): string {
    return JSON.stringify(inArray);
  }

  jsonString2Uint8Array(inString: string): Uint8Array {
    let buffer = [];
    let parsed : object = JSON.parse(inString);
    for (const key of Object.keys(parsed)) {
      buffer.push(parsed[key]);
    }
    return new Uint8Array(buffer);
  }

  string2Uint8Array(inString: string): Uint8Array {
    let buffer = [];
    for (let i = 0; i < inString.length; i++) {
      buffer.push(inString.charCodeAt(i));
    }
    buffer.push(0);
    return new Uint8Array(buffer);
  }

  getStartViewImage() : Resource {
    if (Config.getDeviceType() === 'tablet') {
      return $r('app.media.face_start_view_pad');
    }
    return $r('app.media.face_start_view');
  }
}

let commonController = new CommonController();
export default commonController as CommonController;