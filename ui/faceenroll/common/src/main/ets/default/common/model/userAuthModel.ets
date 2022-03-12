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

import UserIDM from '@ohos.useridm';
import Config from "../Config/Config.ets"

const auth = Config.AUTH
export default class userAuthModel {
  private static userAuthModel = new userAuthModel();
  private authCounts: number;
  private UserIDM ;

  constructor() {
    this.UserIDM = UserIDM.constructor();
  }

  /**
   * Return an instance of userAuthModel.
   */
  public static getInstance(): userAuthModel{
    return this.userAuthModel;
  }

  /**
   * Get userAuthInfo.
   * @param {function} callback - Callback function.
   * @param {function} getUserMode - Do after callback function.
   */
  public getUserAuthInfo(callback, getUserMode) {
    console.info("FaceEnroll userAuthModel getUserAuthInfo start");
    this.UserIDM.getAuthInfo(Config.AUTH_TYPE, (data) => {
      try {
        console.info('FaceEnroll userAuthModel getUserAuthInfo getAuthInfo success', JSON.stringify(data));
        callback(data,getUserMode);
      }
      catch (e) {
        console.info('FaceEnroll userAuthModel getUserAuthInfo getAuthInfo error' + JSON.stringify(e));
      }
    })
    console.info("FaceEnroll userAuthModel getUserAuthInfo end");
  }

  /**
   * Get phone mode(2D or 3D).
   * @param callback
   */
  public getUserMode(callback) {
      callback(Config.TYPE2D);
  }

  /**
   * Delete face.
   * @param {number} credentialId - CredentialId.
   * @param {unit8Array} token - Token.
   * @param callback
   * @param sucssCallback
   */
  public deleteFace(credentialId, token, callback, sucssCallback) {
    console.info("FaceEnroll userAuthModel deleteFace start");
    this.UserIDM.delCred(credentialId, token, {
      onResult: (result, extraInfo) => {
        try {
          console.info('FaceEnroll uaerAuthModel deleteFace delCred result', JSON.stringify(result));
          callback(result);
        }
        catch (e) {
          console.info('FaceEnroll uaerAuthModel deleteFace delCred error', JSON.stringify(e));
        }
      },
      onAcquireInfo: (moduleNum, acquireNum, extraInfo) => {
        console.info('FaceEnroll uaerAuthModel deleteFace delCred onAcquireInfo callback');
      }
    })
    console.info("FaceEnroll userAuthModel deleteFace end");
  }

  /**
   *
   * @param callback
   */
  public getIsCheck(callback) {
      callback(Config.TRUSTED_DEVICE_CHECK)
  }
}
