/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

import Log from './log';

const TAG = 'CommonUtils';

class CommonUtil {
  public setColorOpacity(color: string, opacity: number): string {
    try {
      let r;
      let g;
      let b;
      if (color[0] === '#') {
        color = color.slice(1);
      }
      const RGBA_LENGTH = 8;
      if (color.length === RGBA_LENGTH) {
        const RGB_IN_RGBA_BEGIN = 2;
        const RGB_IN_RGBA_END = 8;
        color = color.slice(RGB_IN_RGBA_BEGIN, RGB_IN_RGBA_END);
      }
      const BASE_16 = 16;
      const MAX_UINT8 = 255;
      const BIT_8 = 8;
      r = (parseInt(color, BASE_16) >> BIT_8 >> BIT_8) & MAX_UINT8;
      g = (parseInt(color, BASE_16) >> BIT_8) & MAX_UINT8;
      b = parseInt(color, BASE_16) & MAX_UINT8;
      let a = opacity;
      let colorTransparency = 'rgba(' + r + ',' + g + ',' + b + ',' + a + ')';
      return colorTransparency;
    } catch (error) {
      Log.error(TAG, `setColorOpacity failed, error code: ${error.code}, message: ${error.message}.`);
    }
    return '';
  }

  public getColor(context: Context, resColor: Resource): string {
    try {
      const BASE_16 = 16;
      let color: string = context.resourceManager.getColorSync(resColor.id).toString(BASE_16);
      Log.info(TAG, `color : ${color}`);
      return color;
    } catch (error) {
      Log.error(TAG, `getColor failed, error Code:${error.code}, message: ${error.message}.`);
    }
    return '';
  }
}

let commonUtil = new CommonUtil();
export default commonUtil;