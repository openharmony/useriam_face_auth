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

import StartViews_2D from "./startViews_2D.ets";
import StartViews_3D from "./startViews_3D.ets";
import Constants from "../common/constant.ets";
import router from '@system.router';

@Entry
@Component
struct Index {
  @StorageLink('show_startViews') show_startViews: string = Constants.SHOW_START_VIEWS_2D;
  private onBackPress(): void{
    AppStorage.Delete('faceToken');
    router.push({
      uri: 'pages/entryView',
    })
  }
  build() {

    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      if (this.show_startViews == Constants.SHOW_START_VIEWS_3D) {
        StartViews_3D()
      }
      else if(this.show_startViews == Constants.SHOW_START_VIEWS_2D)
      {
        StartViews_2D()
      }
    }
    .width('100%')
    .height('100%')
  }
}
