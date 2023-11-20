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
import UserAuthModel from '../model/userAuthModel';
import UserIdmModel from '../model/userIdmModel';
import router from '@system.router';

class EntryViewController {
  private readonly TAG: string = 'EntryViewController';

  constructor() {
    Log.info(this.TAG, 'constructor+');
    UserIdmModel.updateFaceInfo();
    Log.info(this.TAG, 'constructor-');
  }

  async doPINAuth(): Promise<void> {
    try {
      Log.info(this.TAG, 'doPINAuth+');
      let challenge = await UserIdmModel.getChallenge();
      let token = await UserAuthModel.authPin(challenge);
      if (token.length === 0) {
        Log.error(this.TAG, 'authPin fail ');
        throw 1;
      }
      UserIdmModel.setToken(token);
      Log.info(this.TAG, 'set token ok');
    } catch (err) {
      Log.error(this.TAG, 'exception ' + JSON.stringify(err));
      Log.info(this.TAG, 'doPINAuth -');
      throw 1;
    }
    if (UserIdmModel.getFaceNum() === 0) {
      Log.info(this.TAG, 'route to enrollIntro');
      router.replace({uri: 'pages/enrollIntro'});
    } else {
      Log.info(this.TAG, 'route to faceConfig');
      router.replace({uri: 'pages/faceConfig'});
    }

    Promise.resolve(0);
    Log.info(this.TAG, 'doPINAuth -');
  }
}

let entryViewController = new EntryViewController();
export default entryViewController as EntryViewController;