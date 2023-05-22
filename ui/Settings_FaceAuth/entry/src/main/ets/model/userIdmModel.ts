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

import Log from '../utils/log'
import account_osAccount from '@ohos.account.osAccount';
import CommonController from '../controller/commonController'

class UserIdmModel {
  protected readonly TAG: string = "UserIdmModel"
  protected userIdentityManager: account_osAccount.UserIdentityManager
  protected challenge: Uint8Array
  protected token: Uint8Array
  protected credentialId: Uint8Array
  protected faceNumber: number = 0
  protected onAcquireCallback : (number)=>{}

  constructor() {
    Log.info(this.TAG, "constructor+")
    this.userIdentityManager = new account_osAccount.UserIdentityManager()
    Log.info(this.TAG, "constructor-")
  }

  destroy(): void {
    Log.info(this.TAG, "destroy+")
    this.userIdentityManager.closeSession()
    Log.info(this.TAG, "destroy-")
  }

  async getChallenge() : Promise<Uint8Array> {
    return await this.userIdentityManager.openSession()
  }

  setToken(token: Uint8Array): void {
    let tokenStr = CommonController.uin8Array2JsonString(token);
    AppStorage.SetOrCreate<string>("UserIdm_Token", tokenStr)
    Log.info(this.TAG, "setToken ok, size " + token.length)
  }

  getToken() : Uint8Array {
    let tokenStr = AppStorage.Get<string>("UserIdm_Token")
    let token = CommonController.jsonString2Uint8Array(tokenStr)
    Log.info(this.TAG, "getToken ok, size " + token.length)
    return token
  }

  setCredentialId(credId: Uint8Array): void {
    let credIdStr = CommonController.uin8Array2JsonString(credId);
    AppStorage.SetOrCreate<string>("UserIdm_CredId", credIdStr)
    Log.info(this.TAG, "setCredentialId ok, size " + credId.length)
  }

  getCredentialId() : Uint8Array {
    let credIdStr = AppStorage.Get<string>("UserIdm_CredId")
    let credId = CommonController.jsonString2Uint8Array(credIdStr)
    Log.info(this.TAG, "getCredentialId ok, size " + credId.length)
    return credId
  }

  updateFaceInfo(): void {
    this.userIdentityManager.getAuthInfo(2).then((data) => {
      Log.info(this.TAG, 'getAuthInfo length ' + data.length)
      AppStorage.SetOrCreate<number>("UserIdm_FaceNum", data.length);
      if (data.length > 0) {
        this.setCredentialId(data[0].credentialId);
      }
    });
  }

  getFaceNum(): number {
    return AppStorage.Get<number>("UserIdm_FaceNum");
  }

  setOnAcquireCallback(onAcquireCallback: (result: number) => {}): void {
    this.onAcquireCallback = onAcquireCallback
  }

  async enrollFace() : Promise<number> {
    Log.info(this.TAG, "enrollFace+")
    let localToken = this.getToken()
    return new Promise<number>((resolve)=> {
      this.userIdentityManager.addCredential({
        credType: 2,
        credSubType: 10000,
        token: localToken,
      }, {
        onResult: (result, extraInfo) => {
          Log.info(this.TAG, "enrollFace onResult+")
          Log.info(this.TAG, 'FaceEnroll face.addCredential onResult ' + result)
          if (result === 0) {
            this.setCredentialId(extraInfo.credentialId)
            this.updateFaceInfo()
          }
          Log.info(this.TAG, "enrollFace onResult-")
          resolve(result)
        },
        onAcquireInfo(moduleId, number, extraInfo) {
          Log.info(this.TAG, "onAcquireInfo+ " + moduleId + ":" + number + ":" + JSON.stringify(extraInfo))
          this.onAcquireCallback(number)
          Log.info(this.TAG, "onAcquireInfo-")
        }
      })
    })
  }

  async deleteFace() : Promise<number> {
    Log.info(this.TAG, "deleteFace+")
    if (this.credentialId === null) {
      Log.error(this.TAG, "no credential to delete")
      return 0
    }

    let localToken = this.getToken()
    let credId = this.getCredentialId()
    return new Promise<number>((resolve)=> {
      this.userIdentityManager.delCred(credId, localToken, {
          onResult: (result, extraInfo) => {
            Log.info(this.TAG, 'FaceEnroll face.delete onResult result = ' + result);
            this.updateFaceInfo()
            resolve(result)
          }
      })
    })
  }
}

let userIdmModel = new UserIdmModel();
export default userIdmModel as UserIdmModel