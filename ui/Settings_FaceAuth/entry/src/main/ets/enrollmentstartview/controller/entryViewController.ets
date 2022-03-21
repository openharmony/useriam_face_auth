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

import Log from '../utils/log.ets'
import UserAuthModel from '../model/userAuthModel.ets'
import UserIDMModel from '../model/userIDMModel.ets'
import router from '@system.router';

class EntryViewController {
  private TAG: string = "EntryViewController"

  constructor() {
    Log.info(this.TAG, "constructor+")
    UserIDMModel.updateFaceInfo()
    Log.info(this.TAG, "constructor-")
  }

  async doPINAuth() {
    try {
      Log.info(this.TAG, "doPINAuth in")
      let challenge = await UserIDMModel.getChallenge()
      Log.info(this.TAG, "get challenge " + JSON.stringify(challenge) )
      let token = await UserAuthModel.authPin(challenge)
      Log.info(this.TAG, "authPin result " + JSON.stringify(token))
      if (token == null) {
         Log.info(this.TAG, "authPin fail ")
         throw 1
      }
      UserIDMModel.setToken(token)
      Log.info(this.TAG, "set token ok")
    } catch(err) {
      Log.info(this.TAG, "exception " + JSON.stringify(err))
      Log.info(this.TAG, "doPINAuth -")
      throw 1
    }
    if (UserIDMModel.getFaceNum() == 0) {
      Log.info(this.TAG, "route to enrollIntro")
      router.replace({uri: 'pages/enrollIntro'})
    } else {
      Log.info(this.TAG, "route to faceConfig")
      router.replace({uri: 'pages/faceConfig'})
    }

    Promise.resolve(0)
    Log.info(this.TAG, "doPINAuth -")
  }
}

let entryViewController = new EntryViewController();
export default entryViewController as EntryViewController