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
import UserIDMModel from '../model/userIDMModel.ets'
import CommonController from '../controller/commonController.ets'

class FaceConfigController {
  private TAG: string = "FaceConfigController:"

  constructor() {
    Log.info(this.TAG, "constructor+")
    Log.info(this.TAG, "constructor-")
  }

  deleteFace() {
    Log.info(this.TAG, "deleteFace+")
    UserIDMModel.deleteFace().then((result)=>{
      Log.info(this.TAG, "deleteFace result: " + result)
      if (result == 0) {
        Log.info(this.TAG, "deleteFace routeBack+")
        CommonController.routeBack()
        Log.info(this.TAG, "deleteFace routerBack-")
      } else {
        Log.info(this.TAG, "deleteFace fail")
      }
    }).catch(()=>{
      Log.info(this.TAG, "deleteFace exception")
      CommonController.routeBack()
    })
    setTimeout(()=>{CommonController.routeBack()}, 500)

    Log.info(this.TAG, "deleteFace-")
  }
}

let faceConfigController = new FaceConfigController();
export default faceConfigController as FaceConfigController