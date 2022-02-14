/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import Config from "../Config/Config.ets"

export default class ResourceManager {

  private static resourceManager: ResourceManager = new ResourceManager();

  public static getInstance(): ResourceManager{
    return this.resourceManager;
  }

  /**
   * Start pinAuth abillity and get token.
   * @param callback
   * @param getTokenCallback
   */
  public jumpToUserAuth(callback,getTokenCallback) {
      callback(Config.FACE_TOKEN,getTokenCallback);
    //    FeatureAbility.startAbilityForResult({
    //      want: {
    //        bundleName: 'com.ohos....',
    //        abilityName: 'com.ohos...'
    //      },
    //      requestCode: 1,
    //    },
    //      (err, data) => {
    //        console.log("FaceEnroll resourceManager startAbilityForResult asyncCallback StartAbilityResult: "
    //        + err.code + " data: " + data)
    //      }).then(data =>
    //    console.info("FaceEnroll resourceManager startApplicationForResult promise::then : " + JSON.stringify(data));
    //    ).catch(error =>
    //    console.info("FaceEnroll resourceManager startApplicationForResult  promise::catch : " + JSON.stringify(error));
    //    );
  }
}