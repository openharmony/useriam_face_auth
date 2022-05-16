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
import display from '@ohos.display';

class Config {
  private TAG: string = "Config:"
  private deviceType : string = "tablet"
  private nonAppBarDisplayHeight: number = 0
  private nonDisplayHeight: number = 0

  constructor() {
    Log.info(this.TAG, "constructor+")
    display.getDefaultDisplay().then((display)=>{
      if (typeof display.height === "string") {
        Log.info(this.TAG, "mock mode ")
        this.deviceType = "tablet"
        return
      }
      Log.info(this.TAG, "display height: " + display.height +
        " display width: " + display.width)
      if (display.height > display.width) {
        this.deviceType = "phone"
      } else {
        this.deviceType = "tablet"
      }
    })
    Log.info(this.TAG, "constructor-")
  }

  getDeviceType() : string {
    return this.deviceType
  }
}

let config = new Config();
export default config as Config