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

import UserAuthModel from '../model/userAuthModel.ets'
import FeatureAbility from '@ohos.ability.featureAbility';
import ResourceManager from '../manager/resourceManager.ets'
import router from '@system.router';

/**
 * Class UserSettingsPresenter.
 */
export default class UserSettingsPresenter {
  private authLength: number ;
  protected mUserAuthModel: UserAuthModel;
  protected mResourceManager: ResourceManager;
  private deleteResArr = new Array();
  private faceInfo;
  private static userSettingsPresenter: UserSettingsPresenter = new UserSettingsPresenter();
  private static resourceManager: ResourceManager = new ResourceManager();

  protected constructor() {
    this.mUserAuthModel = UserAuthModel.getInstance();
    this.mResourceManager = ResourceManager.getInstance();
    if (this.mUserAuthModel != undefined) {
      this.faceInfo = AppStorage.Get('faceInfo');
    }
  }

  /**
   * Return an instance of UserSettingsPresenter.
   */
  public static getInstance(): UserSettingsPresenter{
    return this.userSettingsPresenter;
  }

  /**
   * Delete face.
   * @param {object} faceInfos - User faceInfos.
   * @param {Uint8Array} token - Token.
   * @param {function} sucssCallback - Do this after callback.
   */
  public deleteFace(faceInfos, token, sucssCallback) {
    for (let i = this.deleteResArr.length; i < faceInfos.length; i++) {
      this.mUserAuthModel.deleteFace(faceInfos[i].credentialId, token, this.deleteFaceCallback.bind(this), sucssCallback);
    }
  }

  /**
   * Delete face callback.
   * @param {boolean} deleteRes - Delete face result.
   * @param {function} sucssCallback - Do this after get delete result.
   */
  public deleteFaceCallback(deleteRes, sucssCallback) {
    let res: boolean = deleteRes;
    this.deleteResArr.push(res);
    if (this.deleteResArr.length == this.faceInfo.length) {
      AppStorage.SetOrCreate('deleteRes', this.deleteResArr);
      sucssCallback();
    }
  }

  /**
   * Delete face pad.
   * @param {object} faceInfos - User faceInfos.
   * @param {Uint8Array} token - Token.
   * @param {function} sucssCallback - Do this after callback.
   */
  public deleteFacePad(faceInfo, token, sucssCallback) {
    this.mUserAuthModel.deleteFace(faceInfo.credentialId, token, this.deleteFaceCallbackPad.bind(this), sucssCallback);
  }
  /**
 * Delete face Pad callback.
 * @param {boolean} deleteRes - Delete face result.
 * @param {function} sucssCallback - Do this after get delete result.
 */
  public deleteFaceCallbackPad(deleteRes, sucssCallback) {
    if (deleteRes == true) {
      sucssCallback();
    }
  }
}

