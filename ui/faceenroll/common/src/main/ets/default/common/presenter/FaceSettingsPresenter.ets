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

import FaceSettingsModel from '../model/FaceSettingsModel.ets';

/**
 * Class FaceSettingsPresenter.
 */
export default class FaceSettingsPresenter {
  private static FaceSettingsPresenter: FaceSettingsPresenter = new FaceSettingsPresenter();
  private mFaceSettingsModel: FaceSettingsModel;

  private constructor() {
    this.mFaceSettingsModel = FaceSettingsModel.getInstance();

  }

  /**
   * Return an instance of FaceSettingsPresenter.
   */
  public static getInstance(): FaceSettingsPresenter{
    return this.FaceSettingsPresenter;
  }

  /**
   * New face data.(pad)
   */
  NewFaceData(callback) {
    console.info("FaceEnroll FaceSettingsPresenter NewFaceData start");
    this.mFaceSettingsModel.NewFaceData((data) => {
      callback(data)
    })
    console.info("FaceEnroll FaceSettingsPresenter NewFaceData end");
  }


  /**
   * Choose unlock method
   *
   * @param {object} methods - The unlock method
   */
  chooseUnlockMethod(method) {
    console.info("FaceEnroll FaceSettingsPresenter chooseUnlockMethod start");
    this.mFaceSettingsModel.chooseUnlockMethod(method);
    console.info("FaceEnroll FaceSettingsPresenter chooseUnlockMethod end");
  }

  /**
   * Whether to start up app lock
   *
   * @param {object} status - The AppLock status
   */
  AppLock(status) {
    console.info("FaceEnroll FaceSettingsPresenter AppLock start");
    this.mFaceSettingsModel.AppLock(status);
    console.info("FaceEnroll FaceSettingsPresenter AppLock end");
  }

  /**
   * start up safe cabinet
   *
   * @param {object} status - The SafeCabinet status
   */
  SafeCabinet(status) {
    console.info("FaceEnroll FaceSettingsPresenter SafeCabinet start");
    this.mFaceSettingsModel.SafeCabinet(status);
    console.info("FaceEnroll FaceSettingsPresenter SafeCabinet end");
  }

  /**
   * Whether to start up  Screen fill light in low light environment
   *
   * @param {object} status - The ScreenFillFightInLowLightEnvironment status
   */
  ScreenFillFightInLowLightEnvironment(status) {
    console.info("FaceEnroll FaceSettingsPresenter ScreenFillFightInLowLightEnvironment start");
    this.mFaceSettingsModel.ScreenFillFightInLowLightEnvironment(status);
    console.info("FaceEnroll FaceSettingsPresenter ScreenFillFightInLowLightEnvironment end");
  }

  /**
   * smart lock screen notifications
   *
   * @param {object} status - The SmartLockScreenNotifications status
   */
  SmartLockScreenNotifications(status) {
    console.info("FaceEnroll FaceSettingsPresenter SmartLockScreenNotifications start");
    this.mFaceSettingsModel.SmartLockScreenNotifications(status);
    console.info("FaceEnroll FaceSettingsPresenter SmartLockScreenNotifications end");
  }

  /**
   * Whether to start up automatic fill in account password
   *
   * @param {object} status - The FillInAccountPassword status
   */
  FillInAccountPassword(status) {
    console.info("FaceEnroll FaceSettingsPresenter FillInAccountPassword start");
    this.mFaceSettingsModel.FillInAccountPassword(status);
    console.info("FaceEnroll FaceSettingsPresenter FillInAccountPassword end");
  }

  /**
   * Whether to start up look at screen during face recognition
   *
   * @param {object} status - The lookAtScreenDuringFaceRecognition status
   */
  lookAtScreenDuringFaceRecognition(status) {
    console.info("FaceEnroll FaceSettingsPresenter lookAtScreenDuringFaceRecognition start");
    this.mFaceSettingsModel.lookAtScreenDuringFaceRecognition(status);
    console.info("FaceEnroll FaceSettingsPresenter lookAtScreenDuringFaceRecognition end");
  }

  /**
   * Whether to start up auxiliary mode
   *
   * @param {object} status - The auxiliaryMode status
   */
  auxiliaryMode(status) {
    console.info("FaceEnroll FaceSettingsPresenter auxiliaryMode start");
    this.mFaceSettingsModel.auxiliaryMode(status);
    console.info("FaceEnroll FaceSettingsPresenter auxiliaryMode end");
  }

  /**
   * Whether to turn off and pick up the phone to turn on the screen
   *
   * @param {object} status - The BrightScreenOfThePhone status
   */
  BrightScreenOfThePhone(status) {
    console.info("FaceEnroll FaceSettingsPresenter BrightScreenOfThePhone start");
    this.mFaceSettingsModel.BrightScreenOfThePhone(status)
    console.info("FaceEnroll FaceSettingsPresenter BrightScreenOfThePhone end");
  }
}