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
import CommonController from '../controller/commonController'
import userIAM_faceAuth from '@ohos.userIAM.faceAuth';
class FaceAuthModel {
  protected TAG: string = "FaceAuthModel"
  protected faceAuthManager : any

  constructor() {
    Log.info(this.TAG, "constructor +")
    this.faceAuthManager = new userIAM_faceAuth.FaceAuthManager()
    Log.info(this.TAG, "constructor -")
  }

  async setSurfaceId(surfaceIdString: string) : Promise<number> {
    Log.info(this.TAG, 'set surface id ' + surfaceIdString);
    return this.faceAuthManager.setSurfaceId(surfaceIdString);
  }

  async clearSurfaceId() {
    Log.info(this.TAG, 'clearSurfaceId+');
    this.setSurfaceId("0")
    Log.info(this.TAG, 'clearSurfaceId-');
  }
}

let faceAuthModel = new FaceAuthModel();
export default faceAuthModel as FaceAuthModel