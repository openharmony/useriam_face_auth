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
import Config from "../Config/Config.ets"
/**
 * Class FaceSettingsModel.
 */
export default class FaceSettingsModel {
  private static mFaceSettingsModel: FaceSettingsModel = new FaceSettingsModel();

  /**
   * Return an instance of FaceSettingsModel.
   */
  public static getInstance(): FaceSettingsModel {
    return this.mFaceSettingsModel;
  }

  /**
   * New face data. (pad)
   *
   * @param {object} callback - The callback from presenter.
   */
  NewFaceData(callback) {
    console.info("FaceEnroll FaceSettingsModel  NewFaceData start");
    let credentialId = Math.random()*1000;
    let faceData = Config.FACE_DATA
    callback(faceData);
    console.info("FaceEnroll FaceSettingsModel  NewFaceData end");
  }


  /**
   * Choose unlock method
   *
   * @param {object} methods - The unlock method
   */
  chooseUnlockMethod(method) {
    console.info('FaceEnroll FaceSettingsModel chooseUnlockMethod start');
    console.info('FaceEnroll FaceSettingsModel chooseUnlockMethod method' + method);
    console.info('FaceEnroll FaceSettingsModel chooseUnlockMethod end ');
  }

  /**
   * Whether to start up app lock
   *
   * @param {object} status - The AppLock status
   */
  AppLock(status) {
    console.info('FaceEnroll FaceSettingsModel activateAppLock start');
    if (status === true) {
      AppStorage.SetOrCreate('appLock_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close AppLock ");
      AppStorage.SetOrCreate('appLock_isOn', false);
    }
    console.info('FaceEnroll FaceSettingsModel activateAppLock end ');
  }

  /**
   * Whether to start up safe cabinet
   *
   * @param {object} status - The SafeCabinet status
   */
  SafeCabinet(status) {
    console.info('FaceEnroll FaceSettingsModel activateSafeCabinet start');
    if (status === true) {
      AppStorage.SetOrCreate('safeCabinet_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close activateSafeCabinet ");
      AppStorage.SetOrCreate('safeCabinet_isOn', false);
    }
    console.info('FaceEnroll FaceSettingsModel activateSafeCabinet end ');
  }


  /**
   * Whether to start up Screen fill light in low light environment
   *
   * @param {object} status - The ScreenFillFightInLowLightEnvironment status
   */
  ScreenFillFightInLowLightEnvironment(status) {
    console.info("FaceEnroll FaceSettingsModel ScreenFillFightInLowLightEnvironment start");
    if (status == true) {
      AppStorage.SetOrCreate('fillLight_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close ScreenFillFightInLowLightEnvironment ");
      AppStorage.SetOrCreate('fillLight_isOn', false);
    }
    console.info("FaceEnroll FaceSettingsModel ScreenFillFightInLowLightEnvironment end");
  }

  /**
   * Whether to start up smart lock screen notifications
   *
   * @param {object} status - The SmartLockScreenNotifications status
   */
  SmartLockScreenNotifications(status) {
    console.info("FaceEnroll FaceSettingsModel SmartLockScreenNotifications start");
    if (status === true) {
      AppStorage.SetOrCreate('smartDisplayLockScreen_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close SmartLockScreenNotifications ");
      AppStorage.SetOrCreate('smartDisplayLockScreen_isOn', false);
    }
    console.info("FaceEnroll FaceSettingsModel SmartLockScreenNotifications end");
  }

  /**
   * Whether to start up automatic fill in account password
   *
   * @param {object} status - The FillInAccountPassword status
   */
  FillInAccountPassword(status) {
    console.info("faceSettingsModel FillInAccountPassword start");
    if (status === true) {
      AppStorage.SetOrCreate('fillInAccount_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close FillInAccountPassword ");
      AppStorage.SetOrCreate('fillInAccount_isOn', false);
    }
    console.info("FaceEnroll FaceSettingsModel FillInAccountPassword end");
  }

  /**
   * Whether to start up look at screen during face recognition
   *
   * @param {object} status - The lookAtScreenDuringFaceRecognition status
   */
  lookAtScreenDuringFaceRecognition(status) {
    console.info("FaceEnroll FaceSettingsModel lookAtScreenDuringFaceRecognition start");
    if (status === true) {
      AppStorage.SetOrCreate('lookAtScreenDuringFaceRecognition_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close lookAtScreenDuringFaceRecognition ");
      AppStorage.SetOrCreate('lookAtScreenDuringFaceRecognition_isOn', false);
    }
    console.info("FaceEnroll FaceSettingsModel lookAtScreenDuringFaceRecognition end");
  }

  /**
   * Whether to start up auxiliary mode
   *
   * @param {object} status - The auxiliaryMode status
   */
  auxiliaryMode(status) {
    console.info("FaceEnroll FaceSettingsModel auxiliaryMode start");
    if (status === true) {
      AppStorage.SetOrCreate('auxiliaryMode_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close auxiliaryMode ");
      AppStorage.SetOrCreate('auxiliaryMode_isOn', false);
    }
    console.info("FaceEnroll FaceSettingsModel auxiliaryMode end");
  }


  /**
   * Whether to turn off and pick up the phone to turn on the screen
   *
   * @param {object} status - The BrightScreenOfThePhone status
   */
  BrightScreenOfThePhone(status) {
    console.info("FaceEnroll FaceSettingsModel BrightScreenOfThePhone start");
    if (status === true) {
      AppStorage.SetOrCreate('BrightScreenOfThePhone_isOn', true);
    } else {
      console.info("FaceEnroll FaceSettingsModel close BrightScreenOfThePhone ");
      AppStorage.SetOrCreate('BrightScreenOfThePhone_isOn', false);
    }
    console.info("FaceEnroll FaceSettingsModel BrightScreenOfThePhone end");
  }
}
