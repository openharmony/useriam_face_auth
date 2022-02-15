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

import faceSettings from '../../../../../../../feature/faceSettings/src/main/ets/default/pages/faceSettings.ets';
import TrustedDevices from '../../../../../../../feature/trustedDevice/src/main/ets/default/pages/index.ets';
import router from '@system.router';

@Entry
@Component
struct userSettings {
  @State authLength: number = 1;
  @State userMode: string = '';
  @StorageLink('facePageFlag') facePageFlag: number = 0;
  private faceInfo;
  private faceToken;

  private aboutToAppear(): void  {
    this.faceInfo = AppStorage.Get('faceInfo');
    this.authLength = AppStorage.Get('authLength');
    this.userMode = AppStorage.Get('userMode');
  }

  private onPageHide(): void {
    router.push({
      uri: 'pages/entryView'
    })
  }
  private onBackPress(): void{
    AppStorage.Delete('faceToken');
    router.push({
      uri: 'pages/entryView',
    })
  }
  onPageShow() {

  }

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      if(this.authLength != 0) {
        if (this.facePageFlag == 0) {
          faceSettings({
            faceLength: this.authLength,
            userMode: this.userMode
          });
        }else if(this.facePageFlag == 1) {
          TrustedDevices();
        }
      }
    }
  }
}