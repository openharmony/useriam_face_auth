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
import userAuthModel from '../../../../../../../common/src/main/ets/default/common/model/userAuthModel.ets';

@Entry
@Component
struct userAuth {
  @State authLength: number = 0;
  @State userMode: string = '';
  private mUserAuthModel: userAuthModel;
  private faceInfo ;
  @State faceToken: string = '';
  @State getAllCallback: boolean = false;

  private aboutToAppear(): void  {
    this.mUserAuthModel = userAuthModel.getInstance();
    this.faceToken = AppStorage.Get('faceToken');
    if (this.mUserAuthModel != undefined) {
      this.mUserAuthModel.getUserAuthInfo(this.getAuthInfoCallback.bind(this),this.getUserMode.bind(this));
    }
  }

/**
   * Get phone mode(2D or 3D).
   */
  public getUserMode() {
    this.mUserAuthModel.getUserMode(this.getUserModeCallback.bind(this));
  }

  onPageShow() {
    if (this.faceToken == '' || this.faceToken == undefined) {
      router.push({
        uri: 'pages/entryView',
      })
    }
  }

/**
   * Get authInfo callback.
   * @param {object} authInfo - Callback data.
   * @param {function} getUserMode - Do this function after callback.
   */
  public getAuthInfoCallback(authInfo,getUserMode) {
    this.faceInfo = authInfo;
    this.authLength = authInfo.length;
    AppStorage.SetOrCreate('authLength',this.authLength);
    AppStorage.SetOrCreate('faceInfo',this.faceInfo);
    AppStorage.SetOrCreate('facePageFlag',0);
    getUserMode();
  }

/**
   * Get phone mode(2D or 3D) callback.
   * @param {string} userMode - Phone mode(2D or 3D).
   */
  public getUserModeCallback(userMode) {
    this.userMode = userMode;
    AppStorage.SetOrCreate('userMode',this.userMode);
    this.getAllCallback = true;

  }

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      if (this.getAllCallback) {
        getRouter({
          authLength: this.authLength
        })

      }
    }
  }
}

@Component
struct getRouter{
  @Prop authLength: number;
  public aboutToAppear() {
    if (this.authLength == 0) {
      router.push({
        uri: 'pages/index',
      })
    }else if(this.authLength > 0) {
      router.push({
        uri: 'pages/userSettings',
      })
    }
  }

  build() {
    Column() {}
  }
}