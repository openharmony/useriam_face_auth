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

import DefaultConstants from '../common/DefaultConstants.ets';
import FaceSettingsPresenter from "../../../../../../../common/src/main/ets/default/common/presenter/FaceSettingsPresenter.ets";
import UserSettingsPresenter from '../../../../../../../common/src/main/ets/default/common/presenter/userSettingsPresenter.ets';
import router from '@system.router';

const FACELENGTH: number = 1;
const USER_MODE: string = '3D';
let mFaceSettingsPresenter: FaceSettingsPresenter;
let mUserSettingsPresenter: UserSettingsPresenter;

@Component
export default struct faceSettings {
  scroller: Scroller = new Scroller();
  @Prop faceLength: number;
  @Prop userMode: string;
  @State mFaceLength: number = 1;
  @StorageLink('deleteRes') @Watch('deleteFaceSuccess') resultArr: [] = [];
  @StorageLink('smartDisplayLockScreen_isOn') smartDisplayLockScreen_isOn: boolean = true;
  @StorageLink('fillInAccount_isOn') fillInAccount_isOn: boolean = false;
  @StorageLink('lookAtScreenDuringFaceRecognition_isOn') lookAtScreenDuringFaceRecognition_isOn: boolean = false;
  @StorageLink('fillLight_isOn') fillLight_isOn: boolean = false;
  @StorageLink('auxiliaryMode_isOn') auxiliaryMode_isOn: boolean = false;
  @State unlock_num: number = 2;
  @State safeCabinet_isOn: boolean = true;
  @State appLock_isOn: boolean = true;

  private aboutToAppear(): void  {
    console.info("FaceEnroll FaceSettings aboutToAppear start");
    console.info('FaceEnroll FaceSettings faceLength = ' + this.faceLength);
    console.info('FaceEnroll FaceSettings userMode = ' + this.userMode);
    mUserSettingsPresenter = UserSettingsPresenter.getInstance();
    mFaceSettingsPresenter = FaceSettingsPresenter.getInstance();
    this.mFaceLength = this.faceLength;
    console.info("FaceEnroll FaceSettings aboutToAppear end");
  }

  SafeCabinetDialogController: CustomDialogController = new CustomDialogController({
    builder: SafeCabinetDialog({ status: $safeCabinet_isOn }),
    autoCancel: false,
    //    offset: {
    //      dx: 0, dy: -30
    //    },
    alignment: DialogAlignment.Bottom,
    //    customStyle: true
  })
  AppLockDialogController: CustomDialogController = new CustomDialogController({
    builder: AppLockDialog({ status: $appLock_isOn }),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    //    customStyle: true
  })
  UnlockDialogController: CustomDialogController = new CustomDialogController({
    builder: UnlockDialog({ num: $unlock_num }),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    //    customStyle: true
  })
  deleteFaceDialogController: CustomDialogController = new CustomDialogController({
    builder: DeleteFaceDialog({ faceNum: this.mFaceLength, confirm: this.onAccept.bind(this) }),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    //    customStyle: true
  })

/**
   * Delete face.
   */
  onAccept() {
    console.info("FaceSettings AppStorage.Get('faceInfo')" + JSON.stringify(AppStorage.Get('faceInfo')));
    let faceInfos = new Array();
    faceInfos = AppStorage.Get('faceInfo');
    let faceToken = AppStorage.Get('faceToken');
    let deleteResult = mUserSettingsPresenter.deleteFace(faceInfos, faceToken, this.deleteFaceSuccess.bind(this));
  }

/**
   * Do after delete face successfully.
   */
  deleteFaceSuccess() {
    console.info('FaceSettings deleteFaceSuccess start');
    let deleteRes: [] = AppStorage.Get('deleteRes');
    let faceInfos = new Array();
    faceInfos = AppStorage.Get('faceInfo');
    if (deleteRes.length == faceInfos.length) {
      let resultSucFlag = 0;
      for (let i = 0; i < deleteRes.length; i++) {
        if (deleteRes[i]) {
          resultSucFlag += 1;
        }
      }
      if (resultSucFlag == deleteRes.length) {
        this.deleteFaceDialogController.close();
        router.push({
          uri: 'pages/entryView'
        })
      } else {
        this.deleteFaceDialogController.close();
        console.info('FaceEnroll faceSettings delete face fail');
      }
    } else {
      this.deleteFaceSuccess();
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Scroll(this.scroller) {
        Column() {
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
            Image($r('app.media.back'))
              .width(DefaultConstants.BACK_ICON_WIDTH)
              .height(DefaultConstants.BACK_ICON_HEIGHT)
            Text($r('app.string.face_checker'))
              .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
              .margin({ left: DefaultConstants.PADDING_30 })
          }
          .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          .height(DefaultConstants.SINGLE_HEIGHT)
          .onClick(() => {
            router.push({
              uri: 'pages/entryView'
            })
          })

          Text($r('app.string.face_recognition_is_used'))
            .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
            .fontColor(DefaultConstants.GREY_FONT_COLOR)
            .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
            .margin({ bottom: DefaultConstants.PADDING_16, top: DefaultConstants.PADDING_24 })
          Column() {
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Text($r('app.string.face_unlock_method'))
                .fontSize(DefaultConstants.LARGE_FONT_SIZE)
              Row() {
                if (this.unlock_num === 2) {
                  Text($r('app.string.face_unlock_method_close'))
                    .fontColor(DefaultConstants.GREY_FONT_COLOR)
                    .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
                } else if (this.unlock_num === 1) {
                  Text($r('app.string.face_recognize_slideunlock_title'))
                    .fontColor(DefaultConstants.GREY_FONT_COLOR)
                    .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
                }
                else if (this.unlock_num === 0) {
                  Text($r('app.string.face_recognize_unlock_title'))
                    .fontColor(DefaultConstants.GREY_FONT_COLOR)
                    .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
                }
                Image($r('app.media.ic_settings_arrow'))
                  .width(DefaultConstants.ICON_WIDTH)
                  .height(DefaultConstants.ICON_HEIGHT)
              }
            }
            .height(DefaultConstants.SINGLE_HEIGHT_)
            .onClick(() => {
              this.UnlockDialogController.open()
            })

            Divider()
              .strokeWidth(DefaultConstants.DIVIDER)
              .color(DefaultConstants.GREY_BACKGROUND_COLOR)
            Flex({
              direction: FlexDirection.Row,
              justifyContent: FlexAlign.SpaceBetween,
              alignItems: ItemAlign.Center
            }) {
              Text($r('app.string.secure_notification_new'))
                .fontSize(DefaultConstants.LARGE_FONT_SIZE)
              Toggle({ type: ToggleType.Switch, isOn: this.smartDisplayLockScreen_isOn })
                .onChange(() => {
                  this.smartDisplayLockScreen_isOn = !this.smartDisplayLockScreen_isOn
                  mFaceSettingsPresenter.SmartLockScreenNotifications(this.smartDisplayLockScreen_isOn);
                })
            }
            .height(DefaultConstants.SINGLE_HEIGHT_)

            Divider()
              .strokeWidth(DefaultConstants.DIVIDER)
              .color(DefaultConstants.GREY_BACKGROUND_COLOR)
            if (this.userMode === USER_MODE) {
              Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
                Text($r('app.string.title_strong_box_pref'))
                  .fontSize(DefaultConstants.LARGE_FONT_SIZE)
                if (this.safeCabinet_isOn === true) {
                  Toggle({ type: ToggleType.Switch, isOn: this.safeCabinet_isOn }).onChange(() => {
                    this.safeCabinet_isOn = false
                  })
                } else {
                  Flex({ justifyContent: FlexAlign.End }) {
                    Toggle({ type: ToggleType.Switch, isOn: this.safeCabinet_isOn })
                      .enabled(false)
                  }.width(DefaultConstants.SINGLE_HEIGHT_)
                  .height(DefaultConstants.WIDTH)
                  .onClick(() => {
                    console.log('lzx====>  SafeCabinetDialogController onchange')
                    if (this.safeCabinet_isOn === false) {
                      this.SafeCabinetDialogController.open();
                    } else {
                      this.safeCabinet_isOn = false
                    }
                  })
                }
              }
              .height(DefaultConstants.SINGLE_HEIGHT_)

              Divider()
                .strokeWidth(DefaultConstants.DIVIDER)
                .color(DefaultConstants.GREY_BACKGROUND_COLOR)
            }
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Text($r('app.string.title_apps_security_pref'))
                .fontSize(DefaultConstants.LARGE_FONT_SIZE)
              if (this.appLock_isOn === true) {
                Toggle({ type: ToggleType.Switch, isOn: this.appLock_isOn })
                  .onChange(() => {
                    this.appLock_isOn = false
                    mFaceSettingsPresenter.AppLock(this.appLock_isOn);
                  })
              } else {
                Flex({ justifyContent: FlexAlign.End }) {
                  Toggle({ type: ToggleType.Switch, isOn: this.appLock_isOn })
                    .enabled(false)
                }.width(DefaultConstants.SINGLE_HEIGHT_)
                .height(DefaultConstants.WIDTH)
                .onClick(() => {
                  if (this.appLock_isOn === false) {
                    this.AppLockDialogController.open();
                  } else {
                    this.appLock_isOn = false
                    mFaceSettingsPresenter.AppLock(this.appLock_isOn);
                  }
                })
              }
            }
            .height(DefaultConstants.SINGLE_HEIGHT_)

            Divider()
              .strokeWidth(DefaultConstants.DIVIDER)
              .color(DefaultConstants.GREY_BACKGROUND_COLOR)
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Start }) {
                Text($r('app.string.face_fingerprint_settings_switch_title'))
                  .fontSize(DefaultConstants.LARGE_FONT_SIZE)
                  .margin({ bottom: DefaultConstants.PADDING_4 })
                Text($r('app.string.face_fingerprint_settings_switch_summary'))
                  .fontColor(DefaultConstants.GREY_FONT_COLOR)
                  .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              }

              Toggle({ type: ToggleType.Switch, isOn: this.fillInAccount_isOn })
                .onChange(() => {
                  this.fillInAccount_isOn = !this.fillInAccount_isOn
                  mFaceSettingsPresenter.FillInAccountPassword(this.fillInAccount_isOn);
                })
            }
            .height(DefaultConstants.DOUBLE_HEIGHT_)
          }
          .borderRadius(DefaultConstants.BORDER_RADIUS)
          .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
          .margin({ bottom: DefaultConstants.PADDING })
          .padding({
            top: DefaultConstants.PADDING_8,
            bottom: DefaultConstants.PADDING_8,
            left: DefaultConstants.PADDING,
            right: DefaultConstants.PADDING
          })

          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
            Text($r('app.string.delete_record_title'))
              .fontColor(Color.Red)
              .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          }
          .borderRadius(DefaultConstants.BORDER_RADIUS)
          .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
          .margin({ bottom: DefaultConstants.PADDING_16 })
          .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          .height(DefaultConstants.SINGLE_HEIGHT)
          .onClick(() => {
            this.deleteFaceDialogController.open();
          })

          if (this.faceLength === FACELENGTH) {
            Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
              Text($r('app.string.enroll_backup_face_title'))
                .fontColor(Color.Blue)
                .fontSize(DefaultConstants.LARGE_FONT_SIZE)
            }
            .borderRadius(DefaultConstants.BORDER_RADIUS)
            .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
            .margin({ bottom: DefaultConstants.PADDING_16 })
            .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
            .height(DefaultConstants.SINGLE_HEIGHT)

            Text($r('app.string.enroll_backup_face_summary'))
              .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              .fontColor(DefaultConstants.GREY_FONT_COLOR)
              .margin({ bottom: DefaultConstants.PADDING_32 })
              .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          } else {
            Text($r('app.string.main_and_backup_faces'))
              .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              .fontColor(DefaultConstants.GREY_FONT_COLOR)
              .margin({ bottom: DefaultConstants.PADDING_32 })
              .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          }
          Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
            Text($r('app.string.face_introdution_title'))
              .fontSize(DefaultConstants.LARGE_FONT_SIZE)
            Row() {
              Text($r('app.string.face_enterance_off'))
                .fontColor(DefaultConstants.GREY_FONT_COLOR)
                .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              Image($r('app.media.ic_settings_arrow'))
                .width(DefaultConstants.ICON_WIDTH)
                .height(DefaultConstants.ICON_HEIGHT)
            }
          }
          .borderRadius(DefaultConstants.BORDER_RADIUS)
          .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
          .margin({ bottom: DefaultConstants.PADDING })
          .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          .height(DefaultConstants.SINGLE_HEIGHT)
          .onClick(() => {
            AppStorage.SetOrCreate('facePageFlag', 1);
          })

          if (this.userMode === USER_MODE) {
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Text($r('app.string.title_face_stare_aux'))
                .fontSize(DefaultConstants.LARGE_FONT_SIZE)
              Toggle({ type: ToggleType.Switch, isOn: this.lookAtScreenDuringFaceRecognition_isOn })
                .onChange(() => {
                  this.lookAtScreenDuringFaceRecognition_isOn = !this.lookAtScreenDuringFaceRecognition_isOn
                  mFaceSettingsPresenter.lookAtScreenDuringFaceRecognition(this.lookAtScreenDuringFaceRecognition_isOn);
                })
            }
            .borderRadius(DefaultConstants.BORDER_RADIUS)
            .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
            .margin({ bottom: DefaultConstants.PADDING_16 })
            .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
            .height(DefaultConstants.SINGLE_HEIGHT)
          } else {
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Text($r('app.string.title_fill_light'))
                .fontSize(DefaultConstants.LARGE_FONT_SIZE)
              Toggle({ type: ToggleType.Switch, isOn: this.fillLight_isOn })
                .onChange(() => {
                  this.fillLight_isOn = !this.fillLight_isOn
                  mFaceSettingsPresenter.ScreenFillFightInLowLightEnvironment(this.fillLight_isOn);
                })
            }
            .borderRadius(DefaultConstants.BORDER_RADIUS)
            .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
            .margin({ bottom: DefaultConstants.PADDING })
            .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
            .height(DefaultConstants.SINGLE_HEIGHT)

            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Start }) {
                Text($r('app.string.auxiliary_mode'))
                  .fontSize(DefaultConstants.LARGE_FONT_SIZE)
                  .margin({ bottom: DefaultConstants.PADDING_4 })
                Text($r('app.string.recognize_without_opening_eyes'))
                  .fontColor(DefaultConstants.GREY_FONT_COLOR)
                  .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              }

              Toggle({ type: ToggleType.Switch, isOn: this.auxiliaryMode_isOn })
                .onChange(() => {
                  this.auxiliaryMode_isOn = !this.auxiliaryMode_isOn
                  mFaceSettingsPresenter.auxiliaryMode(this.auxiliaryMode_isOn);
                })
            }
            .borderRadius(DefaultConstants.BORDER_RADIUS)
            .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
            .margin({ bottom: DefaultConstants.PADDING })
            .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
            .height(DefaultConstants.DOUBLE_HEIGHT)
          }
          Text() {
            Span($r('app.string.face_3dguide_info_2'))
              .fontColor(DefaultConstants.GREY_FONT_COLOR)
            if (this.userMode === USER_MODE) {
              Span($r('app.string.face_guide_3d_announce_link'))
                .fontColor(Color.Blue)
            } else {
              Span($r('app.string.face_guide_2d_announce_link'))
                .fontColor(Color.Blue)
            }
          }
          .fontSize(DefaultConstants.SMALL_FONT_SIZE)
          .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          .onClick(() => {
            router.push({
              uri: 'pages/declare'
            })
          })

          Column() {
            Text($r('app.string.recommend_preference_title'))
              .fontColor(DefaultConstants.GREY_FONT_COLOR)
              .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              .margin({ bottom: DefaultConstants.PADDING_16 })
            Text($r('app.string.face_config_light_01_universal'))
              .fontColor(Color.Blue)
              .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
            if (this.userMode === USER_MODE) {
              Text($r('app.string.annotation_screen_keeps_on'))
                .fontColor(Color.Blue)
                .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
                .margin({ top: DefaultConstants.PADDING_16 })
            }
          }
          .borderRadius(DefaultConstants.BORDER_RADIUS)
          .backgroundColor(DefaultConstants.DARK_GREY_BACKGROUND_COLOR)
          .margin({ bottom: DefaultConstants.PADDING_32, top: DefaultConstants.PADDING_32 })
          .padding({
            bottom: DefaultConstants.PADDING_30,
            top: DefaultConstants.PADDING_30,
            left: DefaultConstants.PADDING,
            right: DefaultConstants.PADDING
          })
          .width(DefaultConstants.WIDTH)
          .alignItems(HorizontalAlign.Start)
        }
        .backgroundColor(DefaultConstants.GREY_BACKGROUND_COLOR)
        .width(DefaultConstants.TOTAL_WIDTH)
        .alignItems(HorizontalAlign.Start)
      }
      .scrollBarWidth(0)
    }
    .width(DefaultConstants.WIDTH)
    .height(DefaultConstants.WIDTH)
    .backgroundColor(DefaultConstants.GREY_BACKGROUND_COLOR)
  }
}

@CustomDialog
struct UnlockDialog {
  @Link num: number
  controller: CustomDialogController;
  @State booleans: boolean[] = [false, false, false]

  aboutToAppear() {
    this.booleans[this.num] = true
  }

  isChecked(num) {
    for (let i = 0;i < this.booleans.length; i++) {
      if (i != num) {
        this.booleans[i] = false
      } else {
        this.booleans[i] = true
      }
    }
  }

  build() {
    Column() {
      Text($r('app.string.face_unlock_method'))
        .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_30 })
      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
        Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Start }) {
          Text($r('app.string.face_recognize_unlock_title'))
            .fontSize(DefaultConstants.LARGE_FONT_SIZE)
            .margin({ bottom: DefaultConstants.PADDING_4 })
          Text($r('app.string.face_recognize_unlock_summary_3d'))
            .fontColor(DefaultConstants.GREY_FONT_COLOR)
            .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
        }

        Toggle({ type: ToggleType.Checkbox, isOn: this.booleans[0] })
          .width(DefaultConstants.RADIO_SIZE)
          .height(DefaultConstants.RADIO_SIZE)
          .onChange(() => {
            this.isChecked(0)
            this.num = 0
            mFaceSettingsPresenter.chooseUnlockMethod(this.num);
            this.controller.close();
          })
      }.height(DefaultConstants.DOUBLE_HEIGHT_)

      Divider()
        .strokeWidth(DefaultConstants.DIVIDER)
        .color(DefaultConstants.GREY_BACKGROUND_COLOR)
      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
        Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Start }) {
          Text($r('app.string.face_recognize_slideunlock_title'))
            .fontSize(DefaultConstants.LARGE_FONT_SIZE)
            .margin({ bottom: DefaultConstants.PADDING_4 })
          Text($r('app.string.face_recognize_slideunlock_summary_3d'))
            .fontColor(DefaultConstants.GREY_FONT_COLOR)
            .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
        }

        Toggle({ type: ToggleType.Checkbox, isOn: this.booleans[1] })
          .width(DefaultConstants.RADIO_SIZE)
          .height(DefaultConstants.RADIO_SIZE)
          .onChange(() => {
            this.isChecked(1)
            this.num = 1
            mFaceSettingsPresenter.chooseUnlockMethod(this.num);
            this.controller.close()
          })
      }.height(DefaultConstants.DOUBLE_HEIGHT_)

      Divider()
        .strokeWidth(DefaultConstants.DIVIDER)
        .color(DefaultConstants.GREY_BACKGROUND_COLOR)
      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
        Text($r('app.string.face_unlock_method_close'))
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        Toggle({ type: ToggleType.Checkbox, isOn: this.booleans[2] })
          .width(DefaultConstants.RADIO_SIZE)
          .height(DefaultConstants.RADIO_SIZE)
          .onChange(() => {
            this.isChecked(2)
            this.num = 2;
            mFaceSettingsPresenter.chooseUnlockMethod(this.num);
            this.controller.close()
          })
      }.height(DefaultConstants.SINGLE_HEIGHT_)

      Column() {
        Text($r('app.string.face_record_cancel'))
          .fontColor(Color.Blue)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
      }
      .width(DefaultConstants.WIDTH)
      .margin({ top: DefaultConstants.PADDING_20 })
      .onClick(() => {
        this.controller.close()
      })
    }
    .width(DefaultConstants.TOTAL_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      bottom: DefaultConstants.PADDING_30,
      top: DefaultConstants.PADDING_20,
      left: DefaultConstants.PADDING_30,
      right: DefaultConstants.PADDING_30
    })
  }
}

@CustomDialog
struct SafeCabinetDialog {
  @Link status: boolean
  controller: CustomDialogController

  build() {
    Column() {
      Text($r('app.string.activate_safe'))
        .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_30 })
      Text($r('app.string.face_inform_open_strongbox_msg'))
        .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_20 })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Text($r('app.string.face_record_cancel'))
          .fontColor(Color.Blue)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            this.controller.close()
          })
        Divider()
          .vertical(true)
          .strokeWidth(DefaultConstants.DIVIDER)
          .color(DefaultConstants.DARK_GREY_BACKGROUND_COLOR)
          .height(DefaultConstants.PADDING_30)
        Text($r('app.string.enable'))
          .fontColor(Color.Blue)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            this.status = true;
            mFaceSettingsPresenter.SafeCabinet(this.status);
            this.controller.close()
          })
      }
    }
    .width(DefaultConstants.TOTAL_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      bottom: DefaultConstants.PADDING_30,
      top: DefaultConstants.PADDING_20,
      left: DefaultConstants.PADDING_30,
      right: DefaultConstants.PADDING_30
    })
  }
}

@CustomDialog
struct AppLockDialog {
  @Link status: boolean
  controller: CustomDialogController

  build() {
    Column() {
      Text($r('app.string.activate_app_lock'))
        .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_30 })
      Text($r('app.string.face_inform_open_applock_msg'))
        .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_20 })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Text($r('app.string.face_record_cancel'))
          .fontColor(Color.Blue)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            this.controller.close()
          })
        Divider()
          .vertical(true)
          .strokeWidth(DefaultConstants.DIVIDER)
          .color(DefaultConstants.DARK_GREY_BACKGROUND_COLOR)
          .height(DefaultConstants.PADDING_30)
        Text($r('app.string.enable'))
          .fontColor(Color.Blue)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            this.status = true;
            mFaceSettingsPresenter.AppLock(this.status);
            this.controller.close()
          })
      }
    }
    .width(DefaultConstants.TOTAL_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      bottom: DefaultConstants.PADDING_30,
      top: DefaultConstants.PADDING_20,
      left: DefaultConstants.PADDING_30,
      right: DefaultConstants.PADDING_30
    })
  }
}

@CustomDialog
struct DeleteFaceDialog {
  controller: CustomDialogController;
  @Prop faceNum: number;
  @StorageLink('BrightScreenOfThePhone_isOn') BrightScreenOfThePhone_isOn: boolean = true;
  confirm: () => void;

  aboutToAppear() {
    console.log('FaceSettings faceNum ' + this.faceNum)
  }

  build() {
    Column() {
      if (this.faceNum === FACELENGTH) {
        Text($r('app.string.delete_record_title'))
          .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
          .margin({ bottom: DefaultConstants.PADDING_20 })
      } else {
        Text($r('app.string.delete_all_face_data'))
          .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
          .margin({ bottom: DefaultConstants.PADDING_20 })
      }
      Text($r('app.string.wether_delete_face_data'))
        .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.PADDING_16 })
      Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
        Toggle({ type: ToggleType.Checkbox, isOn: this.BrightScreenOfThePhone_isOn })
          .width(DefaultConstants.RADIO_SIZE)
          .height(DefaultConstants.RADIO_SIZE)
          .onChange((isOn) => {
            this.BrightScreenOfThePhone_isOn = !this.BrightScreenOfThePhone_isOn
          })
        Text($r('app.string.face_recognize_lighton_close_universal'))
          .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
      }
      .margin({ left: -DefaultConstants.PADDING_10, bottom: DefaultConstants.PADDING_20 })

      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Text($r('app.string.face_record_cancel'))
          .fontColor(Color.Blue)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            this.controller.close()
          })
        Divider()
          .vertical(true)
          .strokeWidth(DefaultConstants.DIVIDER)
          .color(DefaultConstants.DARK_GREY_BACKGROUND_COLOR)
          .height(DefaultConstants.PADDING_30)
        Text($r('app.string.face_record_delete'))
          .fontColor(Color.Red)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
          .onClick(() => {
            mFaceSettingsPresenter.BrightScreenOfThePhone(this.BrightScreenOfThePhone_isOn);
            this.controller.close()
            this.confirm()
          })
      }
    }
    .width(DefaultConstants.TOTAL_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      bottom: DefaultConstants.PADDING_30,
      top: DefaultConstants.PADDING_20,
      left: DefaultConstants.PADDING_30,
      right: DefaultConstants.PADDING_30
    })
  }
}
