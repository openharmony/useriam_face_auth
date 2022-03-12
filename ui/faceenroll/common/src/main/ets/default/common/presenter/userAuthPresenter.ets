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

import ResourceManager from '../manager/resourceManager.ets'
import router from '@system.router';

export default class UserAuthPresenter {
  protected mResourceManager: ResourceManager;
  private static userAuthPresenter: UserAuthPresenter = new UserAuthPresenter();
  private static resourceManager: ResourceManager = new ResourceManager();

  protected constructor() {
    this.mResourceManager = ResourceManager.getInstance();
  }

  public static getInstance(): UserAuthPresenter{
    return this.userAuthPresenter;
  }

  /**
   * Jump to userAuth page.
   * @param callback
   */
  public jumpToUserAuth(callback) {
    this.mResourceManager.jumpToUserAuth(this.jumpToUserAuthCallBack.bind(this),callback);
  }

  /**
   * Jump to userAuth after got token.
   * @param {unit8Array} token - Token.
   * @param {function} getTokenCallback - Do after get token.
   */
  public jumpToUserAuthCallBack(token,getTokenCallback) {
    AppStorage.SetOrCreate('faceToken', token);
    getTokenCallback();
    setTimeout(()=>{
      router.push({
        uri:'pages/entryView'
      });
      AppStorage.Delete('faceToken');
    },600000);
  }
}

