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
 
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';
import CommonUtil from '../../../main/ets/utils/CommonUtil';

  export default function commonUtilTest() {
  describe('commonUtilTest', () => {
    it('setColorOpacityTest', 0, () => {
      let color1 = '#a53eda';
      let colorOpacity1 = CommonUtil.setColorOpacity(color1, 0.5);
      expect('rgba(165,62,218,0.5)').assertEqual(colorOpacity1);

      let color2 = 'a53eda';
      let colorOpacity2 = CommonUtil.setColorOpacity(color2, 0.5);
      expect('rgba(165,62,218,0.5)').assertEqual(colorOpacity2);

      let color3 = '#ffa53eda';
      let colorOpacity3 = CommonUtil.setColorOpacity(color3, 0.5);
      expect('rgba(165,62,218,0.5)').assertEqual(colorOpacity3);

      let color4 = 'ffa53eda';
      let colorOpacity4 = CommonUtil.setColorOpacity(color4, 0.5);
      expect('rgba(165,62,218,0.5)').assertEqual(colorOpacity4);
    })

  })
}