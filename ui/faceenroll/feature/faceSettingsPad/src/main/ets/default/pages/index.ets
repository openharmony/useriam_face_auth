// @ts-nocheck
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
import DefaultConstants from '../common/DefaultConstants.ets'
import FaceSettingsPresenter from "../../../../../../../common/src/main/ets/default/common/presenter/FaceSettingsPresenter.ets"
import UserSettingsPresenter from '../../../../../../../common/src/main/ets/default/common/presenter/userSettingsPresenter.ets'

let mFaceSettingsPresenter: FaceSettingsPresenter;
let mUserSettingsPresenter: UserSettingsPresenter;
const FACENAME = '面部';

@Component
export default struct FaceSettings {
  scroller: Scroller = new Scroller();
  @Prop faceLength: number;
  @Prop userMode: string;
  @StorageLink('faceInfo') faceInfos: [] = [];
  @StorageLink('fillLight_isOn') fillLight_isOn: boolean = false;
  @StorageLink('fillInAccount_isOn') fillInAccount_isOn: boolean = false;
  @State appLock_isOn: boolean = true;
  @State unlock_num: number = 2;
  @State text: string = '面部1';
  private removeFace ;
  @State faceNames: [] = [];
  private name: [] = [{ isUsed:false }, { isUsed:false },{ isUsed:false },{ isUsed:false },{ isUsed:false }];

  private aboutToAppear(): void  {
    console.info("FaceEnroll FaceSettings aboutToAppear start");
    console.info('FaceEnroll FaceSettings faceLength = ' + this.faceLength);
    console.info('FaceEnroll FaceSettings userMode = ' + this.userMode);
    mFaceSettingsPresenter = FaceSettingsPresenter.getInstance();
    mUserSettingsPresenter = UserSettingsPresenter.getInstance();
    for (let i = 0; i < this.faceInfos.length; i++) {
      this.faceNames.push({
        id: this.faceInfos[i].credentialId,
        name: i+1
      });
      this.name[i].isUsed = true;
    }
    console.info("FaceEnroll FaceSettings aboutToAppear end");
  }

  AppLockDialogController: CustomDialogController = new CustomDialogController({
    builder: AppLockDialog({ status: $appLock_isOn }),
    autoCancel: false,
    customStyle: true
  })
  UnlockDialogController: CustomDialogController = new CustomDialogController({
    builder: UnlockDialog({ num: $unlock_num }),
    autoCancel: false,
    customStyle: true
  })
  ReviseFaceController: CustomDialogController = new CustomDialogController({
    builder: ReviseFaceDialog({ text: $text, confirm: this.onAccept.bind(this) }),
    autoCancel: false,
    customStyle: true
  })

  onAccept() {
    let faceToken = AppStorage.Get('faceToken');
    mUserSettingsPresenter.deleteFacePad(this.removeFace, faceToken, this.deleteFaceSuccess.bind(this));
  }

  deleteFaceSuccess() {
    if (this.faceInfos.length != 0) {
//      let index = this.faceInfos.indexOf(this.removeFace)
//      this.faceInfos.splice(index, 1)
      for(let i =0;i<this.faceNames.length;i++){
        if(this.removeFace ==this.faceNames[i].id ){
          this.name[(this.faceNames[i].name-1)].isUsed = false;
          this.faceNames.splice(i, 1)
        }
      }
      for(let i =0;i<this.faceInfos.length;i++){
        if(this.removeFace ==this.faceInfos[i].credentialId ){
          this.faceInfos.splice(i, 1)
        }
      }
    }
  }

  addFaceInfo(faceInfo) {
    this.faceInfos.push(faceInfo)
    console.info('FaceSettings faceInfos = ' + this.faceInfos.length);
    let addName ;
    for(let i =0;i<this.name.length;i++){
      if(this.name[i].isUsed==false){
        addName = i+1;
        this.name[i].isUsed = true;
        break;
      }
    }
    this.faceNames.push({
      id:faceInfo.credentialId,
      name: addName
    })
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

          Text($r('app.string.face_recognition_is_used'))
            .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
            .fontColor(DefaultConstants.GREY_FONT_COLOR)
            .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
            .margin({ bottom: DefaultConstants.TEXT_PADDING, top: DefaultConstants.TEXT_PADDING2 })
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
              this.UnlockDialogController.open();
            })

            Divider()
              .strokeWidth(DefaultConstants.DIVIDER)
              .color(DefaultConstants.GREY_BACKGROUND_COLOR)
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Text($r('app.string.title_apps_security_pref'))
                .fontSize(DefaultConstants.LARGE_FONT_SIZE)
              if (this.appLock_isOn === true) {
                Row() {
                  Toggle({ type: ToggleType.Switch, isOn: this.appLock_isOn })
                    .width(DefaultConstants.TOGGLE_SIZE)
                    .onChange(() => {
                      this.appLock_isOn = false;
                      mFaceSettingsPresenter.AppLock(this.appLock_isOn);
                    })
                }.width(DefaultConstants.TOGGLE_BOX_SIZE)
                .height(DefaultConstants.SINGLE_HEIGHT_)
              } else {
                Row() {
                  Toggle({ type: ToggleType.Switch, isOn: this.appLock_isOn })
                    .width(DefaultConstants.TOGGLE_SIZE)
                    .enabled(false)

                }.width(DefaultConstants.TOGGLE_BOX_SIZE)
                .height(DefaultConstants.SINGLE_HEIGHT_)

                .onClick(() => {
                  if (this.appLock_isOn === false) {
                    this.AppLockDialogController.open();
                  } else {
                    this.appLock_isOn = false;
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
                .width(DefaultConstants.TOGGLE_SIZE)
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
            top: DefaultConstants.PADDING_4,
            bottom: DefaultConstants.PADDING_4,
            left: DefaultConstants.PADDING,
            right: DefaultConstants.PADDING
          })

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

          Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
            Text($r('app.string.title_fill_light'))
              .fontSize(DefaultConstants.LARGE_FONT_SIZE)
            Toggle({ type: ToggleType.Switch, isOn: this.fillLight_isOn })
              .width(DefaultConstants.TOGGLE_SIZE)
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

          Text() {
            Span($r('app.string.multiface_list_title'))
              .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              .fontColor(DefaultConstants.GREY_FONT_COLOR)
            if (Number(this.faceInfos.length) != 0) {
              Span('(' + Number(this.faceInfos.length) + '/5)')
                .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
                .fontColor(DefaultConstants.GREY_FONT_COLOR)
            }
          }
          .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          .margin({ bottom: DefaultConstants.TEXT_PADDING, top: DefaultConstants.TEXT_PADDING })

          Column() {
            ForEach(this.faceNames, (item) => {
              Column() {
                Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
                  Text(FACENAME + item.name)
                    .fontSize(DefaultConstants.LARGE_FONT_SIZE)
                }
                .height(DefaultConstants.SINGLE_HEIGHT_)
                .onClick(() => {
                  this.text = FACENAME + item.name;
                  this.removeFace = item.id;
                  this.ReviseFaceController.open();
                })

                if (this.faceInfos.length <= 4) {
                  Divider()
                    .strokeWidth(DefaultConstants.DIVIDER)
                    .color(DefaultConstants.GREY_BACKGROUND_COLOR)
                }
              }
            })
            if (this.faceInfos.length != 5) {
              Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
                Text($r('app.string.multiface_enroll_new_face'))
                  .fontSize(DefaultConstants.LARGE_FONT_SIZE)
                  .fontColor(DefaultConstants.BLUE_FONT_COLOR)
              }
              .height(DefaultConstants.SINGLE_HEIGHT_)
              .onClick(() => {
                mFaceSettingsPresenter.NewFaceData(this.addFaceInfo.bind(this));
              })
            }
          }
          .borderRadius(DefaultConstants.BORDER_RADIUS)
          .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
          .padding({
            top: DefaultConstants.PADDING_4,
            bottom: DefaultConstants.PADDING_4,
            left: DefaultConstants.PADDING,
            right: DefaultConstants.PADDING
          })

          Text() {
            Span($r('app.string.face_3dguide_info_2'))
              .fontColor(DefaultConstants.GREY_FONT_COLOR)
            Span($r('app.string.face_guide_2d_announce_link'))
              .fontColor(DefaultConstants.BLUE_FONT_COLOR)
          }
          .fontSize(DefaultConstants.SMALL_FONT_SIZE)
          .padding({ left: DefaultConstants.PADDING, right: DefaultConstants.PADDING })
          .margin({ bottom: DefaultConstants.PADDING, top: DefaultConstants.TEXT_PADDING })

          Column() {
            Text($r('app.string.recommend_preference_title'))
              .fontColor(DefaultConstants.GREY_FONT_COLOR)
              .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
              .margin({ bottom: DefaultConstants.GERY_BORDER_PADDING })
            Text($r('app.string.face_config_light_01_universal'))
              .fontColor(DefaultConstants.BLUE_FONT_COLOR)
              .fontSize(DefaultConstants.MEDIUM_FONT_SIZE)
          }
          .borderRadius(DefaultConstants.BORDER_RADIUS)
          .backgroundColor(DefaultConstants.DARK_GREY_BACKGROUND_COLOR)
          .margin({ top: DefaultConstants.PADDING })
          .padding({
            bottom: DefaultConstants.GERY_BORDER_PADDING,
            top: DefaultConstants.GERY_BORDER_PADDING,
            left: DefaultConstants.PADDING,
            right: DefaultConstants.PADDING
          })
          .width(DefaultConstants.WIDTH)
          .alignItems(HorizontalAlign.Start)
        }
        //        .backgroundColor(DefaultConstants.GREY_BACKGROUND_COLOR)
        .padding({
          left: DefaultConstants.PADDING,
          right: DefaultConstants.PADDING
        })
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
struct ReviseFaceDialog {
  controller: CustomDialogController;
  @Link text: string;
  confirm: () => {}

  build() {
    Column() {
      Text($r('app.string.multiface_face_name_title'))
        .fontSize(DefaultConstants.OVERSIZE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.DIALOG_TITLE_BOTTOM })
      TextInput({ placeholder: '面部1', text: this.text })
        .margin({ bottom: DefaultConstants.DIALOG_TITLE_TOP })
        .placeholderFont({ size: DefaultConstants.LARGE_FONT_SIZE })
        .caretColor(Color.White)
        .placeholderColor(Color.Grey)
        .width(DefaultConstants.WIDTH)
        .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
        .onChange((value: string) => {
          this.text = value;
        });
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Button() {
          Text($r('app.string.delete_face'))
            .fontColor(DefaultConstants.BLUE_FONT_COLOR)
            .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        }
        .type(ButtonType.Capsule)
        .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
        .width(DefaultConstants.DIALOG_BUTTON_WIDTH)
        .height(DefaultConstants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.controller.close();
          this.confirm();
        })

        Button() {
          Text($r('app.string.multiface_save'))
            .fontColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
            .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        }
        .backgroundColor(DefaultConstants.BLUE_FONT_COLOR)
        .type(ButtonType.Capsule)
        .width(DefaultConstants.DIALOG_BUTTON_WIDTH)
        .height(DefaultConstants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.controller.close();
        })
      }
    }
    .width(DefaultConstants.DIALOG_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      top: DefaultConstants.DIALOG_TITLE_TOP,
      bottom: DefaultConstants.DIALOG_TITLE_TOP,
      left: DefaultConstants.DIALOG_PADDING,
      right: DefaultConstants.DIALOG_PADDING
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
        .margin({ bottom: DefaultConstants.DIALOG_TITLE_BOTTOM })
      Text($r('app.string.face_inform_open_applock_msg'))
        .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        .margin({ bottom: DefaultConstants.DIALOG_TITLE_TOP })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Button() {
          Text($r('app.string.face_record_cancel'))
            .fontColor(DefaultConstants.BLUE_FONT_COLOR)
            .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        }
        .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
        .type(ButtonType.Capsule)
        .width(DefaultConstants.DIALOG_BUTTON_WIDTH)
        .height(DefaultConstants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.controller.close();
        })

        Divider()
          .vertical(true)
          .strokeWidth(DefaultConstants.DIVIDER)
          .color(DefaultConstants.DARK_GREY_BACKGROUND_COLOR)
          .height(DefaultConstants.PADDING_30)
        Button() {
          Text($r('app.string.enable'))
            .fontColor(DefaultConstants.BLUE_FONT_COLOR)
            .fontSize(DefaultConstants.LARGE_FONT_SIZE)
        }
        .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
        .type(ButtonType.Capsule)
        .width(DefaultConstants.DIALOG_BUTTON_WIDTH)
        .height(DefaultConstants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.status = true;
          mFaceSettingsPresenter.AppLock(this.status);
          this.controller.close();
        })
      }
    }
    .width(DefaultConstants.DIALOG_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      top: DefaultConstants.DIALOG_TITLE_TOP,
      bottom: DefaultConstants.DIALOG_TITLE_TOP,
      left: DefaultConstants.DIALOG_PADDING,
      right: DefaultConstants.DIALOG_PADDING
    })
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
        .margin({ bottom: DefaultConstants.DIALOG_TITLE_BOTTOM })
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

      Button() {
        Text($r('app.string.face_record_cancel'))
          .fontColor(DefaultConstants.BLUE_FONT_COLOR)
          .fontSize(DefaultConstants.LARGE_FONT_SIZE)
      }
      .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
      .type(ButtonType.Capsule)
      .width(DefaultConstants.DIALOG_BUTTON_WIDTH2)
      .height(DefaultConstants.DIALOG_BUTTON_HEIGHT)
      .onClick(() => {
        this.controller.close();
      })
    }
    .width(DefaultConstants.DIALOG_WIDTH)
    .borderRadius(DefaultConstants.BORDER_RADIUS)
    .backgroundColor(DefaultConstants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      top: DefaultConstants.DIALOG_TITLE_TOP,
      bottom: DefaultConstants.DIALOG_TITLE_TOP,
      left: DefaultConstants.DIALOG_PADDING,
      right: DefaultConstants.DIALOG_PADDING
    })
  }
}