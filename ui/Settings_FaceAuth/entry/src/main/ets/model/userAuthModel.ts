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
import CommonController from '../controller/commonController';
import account_osAccount from '@ohos.account.osAccount';

class UserAuthModel {
  protected readonly TAG: string = 'UserAuthModel';
  protected pinAuthManger : account_osAccount.PINAuth;
  protected userAuthManager : account_osAccount.UserAuth;
  private readonly CMD_SET_SURFACE_ID = 100;

  constructor() {
    Log.info(this.TAG, 'constructor+');
    this.pinAuthManger = new account_osAccount.PINAuth();
    this.userAuthManager = new account_osAccount.UserAuth();
    Log.info(this.TAG, 'constructor-');
  }

  async authPin(challenge: Uint8Array):Promise<Uint8Array> {
    Log.info(this.TAG, 'auth Pin+');
    this.pinAuthManger.registerInputer({
      onGetData: (authSubType, iInputData) => {
        Log.info(this.TAG, 'FaceEnroll pin.registerInputer start');
        const SIX_DIGITS_PIN = 10000;
        const PIN_ASCII_LIST = [49, 50, 51, 52, 53, 54];
        iInputData.onSetData(SIX_DIGITS_PIN, new Uint8Array(PIN_ASCII_LIST));
      }
    });

    let ret = new Promise<Uint8Array>((resolve)=> {
      const AUTH_TYPE = account_osAccount.AuthType.PIN;
      const AUTH_TRUST_LEVEL = account_osAccount.AuthTrustLevel.ATL1;
      this.userAuthManager.auth(challenge, AUTH_TYPE, AUTH_TRUST_LEVEL, {
        onResult: (result, extraInfo) => {
          Log.info(this.TAG, 'authPin onResult: ' + result);
          this.pinAuthManger.unregisterInputer();
          if ((result === 0) && extraInfo.token && (Object.keys(extraInfo.token).length > 0)) {
            resolve(extraInfo.token);
            Log.info(this.TAG, 'authPin success');
            const TEN_MINUTE = 10;
            const SIXTY_SECOND = 60;
            const THOUSAND_MS = 1000;
            setTimeout(() => {
              CommonController.terminateAbility();
            }, TEN_MINUTE * SIXTY_SECOND * THOUSAND_MS);
          } else {
            Log.error(this.TAG, 'authPin fail');
            resolve(new Uint8Array([]));
          }
        }
      });
    });
    Log.info(this.TAG, 'auth Pin-');
    return ret;
  }
}

let userAuthModel = new UserAuthModel();
export default userAuthModel as UserAuthModel;