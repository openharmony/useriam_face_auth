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
import trustedModel from "../../../../../../../common/src/main/ets/default/common/model/DeviceModel";
import userAuthModel from "../../../../../../../common/src/main/ets/default/common/model/userAuthModel";
import constants from "../common/DefaultConstants.ets";
import router from '@system.router';

@CustomDialog
struct useTipDialog {
  controller: CustomDialogController;
  cancel: () => void;
  confirm: () => void;

  build() {
    Column() {
      Text($r('app.string.using_tips'))
        .fontSize(constants.OPTION_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TITLE_BOTTOM })
      Text($r('app.string.add_device_tip'))
        .fontSize(constants.TEXT_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TITLE_TOP })
      Text($r("app.string.add_device_or_not_tip"))
        .fontSize(constants.TEXT_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TEXT_BOTTOM })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Button() {
          Text($r('app.string.cancel_button'))
            .fontColor(constants.BLUE_FONT_COLOR)
            .fontSize(constants.TEXT_FONT_SIZE)
        }
        .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
        .type(ButtonType.Capsule)
        .width(constants.DIALOG_BUTTON_WIDTH)
        .height(constants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.controller.close()
          this.cancel()
        })

        Divider()
          .vertical(true)
          .strokeWidth(constants.DIALOG_DRIVER_STROKE_WIDTH)
          .color(constants.GREY_BACKGROUND_COLOR)
          .height(constants.DIALOG_DRIVER_HEIGHT)
        Button() {
          Text($r('app.string.add_device_button'))
            .fontColor(constants.BLUE_FONT_COLOR)
            .fontSize(constants.TEXT_FONT_SIZE)
        }
        .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
        .type(ButtonType.Capsule)
        .width(constants.DIALOG_BUTTON_WIDTH)
        .height(constants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.controller.close()
          this.confirm()
        })
      }
    }
    //    .width(constants.DIALOG_WIDTH)
    .width(constants.MAX_WIDTH)
    .borderRadius(constants.ITEM_BORDER_RADIO)
    .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      top: constants.DIALOG_TITLE_TOP,
      bottom: constants.DIALOG_TITLE_TOP,
      left: constants.DIALOG_PADDING,
      right: constants.DIALOG_PADDING
    })
  }
}

@CustomDialog
struct addDeviceDialog {
  private trustedDeviceModel: trustedModel;
  useTipDialogController: CustomDialogController = new CustomDialogController({
    builder: useTipDialog({ confirm: this.isAddDevice.bind(this) }),
    autoCancel: false
  })
  @State addDeviceList: [{
    name: string,
    isLock: boolean
  }]= [{ name: 'test', isLock: true }];
  @Link addDevice: {
    name: string,
    isLock: boolean
  }
  private selectDevice;
  controller: CustomDialogController;
  confirm: () => void;

  aboutToAppear() {
    this.trustedDeviceModel = trustedModel.getInstance();
    this.trustedDeviceModel.getAddDeviceList(this.getAddDeviceList.bind(this));
  }

  isAddDevice() {
    this.addDevice = this.selectDevice;
    this.confirm();
  }

  getAddDeviceList(addList) {
    this.addDeviceList = addList;
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start }) {
      Text($r("app.string.face_introdution_button"))
        .fontSize(constants.OPTION_FONT_SIZE)
        .height(constants.DIALOG_TITLE_HEIGHT)
      List() {
        ForEach(this.addDeviceList, (item) => {
          ListItem() {
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Start }) {
                Text(item.name)
                  .fontSize(constants.TEXT_FONT_SIZE)
                  .fontColor(item.isLock ? constants.BLACK_FONT_COLOR : constants.LIGHT_GREY_FONT_COLOR)
                  .margin({ bottom: constants.PADDING_4 })
                Text(item.isLock ? $r("app.string.connect_tip") : $r("app.string.disconnect_tip"))
                  .fontColor(item.isLock ? constants.GREY_FONT_COLOR : constants.LIGHT_GREY_FONT_COLOR2)
                  .fontSize(constants.MIN_FONT_SIZE)
              }

              Image($r("app.media.into_arrow"))
                .width(constants.SMALL_INFO_ARROW_WIDTH)
                .objectFit(ImageFit.Contain)
            }
            .height(constants.CONTENT_TIPS_HEIGHT)
          }.editable(false)
          .width(constants.MAX_WIDTH)
          .onClick(() => {
            if (item.isLock) {
              this.selectDevice = item;
              this.controller.close();
              this.useTipDialogController.open();
            }
          })
        })
      }
      .divider({ color: constants.GREY_BACKGROUND_COLOR, strokeWidth: constants.DIALOG_DRIVER_STROKE_WIDTH })
    }
    .width(constants.MAX_WIDTH)
    .borderRadius(constants.ITEM_BORDER_RADIO)
    .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
    .height(this.addDeviceList.length * constants.LIST_ITEM_HEIGHT_ + constants.DIALOG_TITLE_HEIGHT + 2*constants.DIALOG_TITLE_TOP)
    .constraintSize({ maxHeight: 4 * constants.LIST_ITEM_HEIGHT_ + constants.DIALOG_TITLE_HEIGHT + 2*constants.DIALOG_TITLE_TOP })
    .padding({
      bottom: constants.DIALOG_TITLE_TOP,
      top: constants.DIALOG_TITLE_TOP,
      left: constants.DIALOG_PADDING,
      right: constants.DIALOG_PADDING
    })
  }
}


@CustomDialog
struct removeDeviceDialog {
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void
  build() {
    Column() {
      Text($r('app.string.remove_device_title'))
        .fontSize(constants.OPTION_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TITLE_BOTTOM })
      Text($r('app.string.remove_device_tip'))
        .fontSize(constants.TEXT_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TEXT_BOTTOM })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Button() {
          Text($r('app.string.cancel_button'))
            .fontColor(constants.BLUE_FONT_COLOR)
            .fontSize(constants.TEXT_FONT_SIZE)
        }
        .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
        .type(ButtonType.Capsule)
        .width(constants.DIALOG_BUTTON_WIDTH)
        .height(constants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.controller.close()
          this.cancel()
        })

        Divider()
          .vertical(true)
          .strokeWidth(constants.DIALOG_DRIVER_STROKE_WIDTH)
          .color(constants.GREY_BACKGROUND_COLOR)
          .height(constants.DIALOG_DRIVER_HEIGHT)
        Button() {
          Text($r('app.string.remove_device_button'))
            .fontColor(constants.RED_FONT_COLOR)
            .fontSize(constants.TEXT_FONT_SIZE)
        }
        .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
        .type(ButtonType.Capsule)
        .width(constants.DIALOG_BUTTON_WIDTH)
        .height(constants.DIALOG_BUTTON_HEIGHT)
        .onClick(() => {
          this.controller.close()
          this.confirm()
        })
      }
    }
    //    .width(constants.DIALOG_WIDTH)
    .width(constants.MAX_WIDTH)
    .borderRadius(constants.ITEM_BORDER_RADIO)
    .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      top: constants.DIALOG_TITLE_TOP,
      bottom: constants.DIALOG_TITLE_TOP,
      left: constants.DIALOG_PADDING,
      right: constants.DIALOG_PADDING
    })
  }
}

@Component
export default struct trustedDevice {
  private trustedDeviceModel: trustedModel;
  private userAuthModel: userAuthModel;
  @State isUpdate: boolean = false;
  Scroll: Scroller = new Scroller();
  @State isCheck: boolean = false;
  @State deviceList: [{
    name: string,
    isLock: boolean
  }]= [{ name: 'test', isLock: true }];
  @State ifCallback: boolean = false;
  @StorageLink('deviceListLength') deviceListLength: number = 1;
  @State addDevice: {
    name: string,
    isLock: boolean
  } = { name: "", isLock: true };
  delDevice: {
    name: string,
    isLock: boolean
  } = { name: "", isLock: true };
  removeDeviceDialogController: CustomDialogController = new CustomDialogController({
    builder: removeDeviceDialog({ cancel: this.onCancel, confirm: this.removeDevice.bind(this) }),
    cancel: this.onCancel,
    autoCancel: false,
    //    customStyle: true

  })
  addDeviceDialogController: CustomDialogController = new CustomDialogController({
    builder: addDeviceDialog({ addDevice: $addDevice, confirm: this.isAddDevice.bind(this) }),
    cancel: this.onCancel,
    autoCancel: true,
    //    customStyle: true
  })

  aboutToAppear() {
    this.trustedDeviceModel = trustedModel.getInstance();
    this.userAuthModel = userAuthModel.getInstance();
    this.userAuthModel.getIsCheck(this.getIsCheck.bind(this));
  }

  getIsCheck(isCheck) {
    this.isCheck = isCheck;
  }

  getDeviceList(deviceList) {
    this.deviceList = deviceList;
    this.ifCallback = true;
    AppStorage.SetOrCreate('deviceListLength', deviceList.length);
  }

  onCancel() {
  }

  isAddDevice() {
    this.deviceList.push(this.addDevice);
    this.trustedDeviceModel.addDevice(this.addDevice);
  }

  removeDevice() {
    let index = this.deviceList.indexOf(this.delDevice);
    this.deviceList.splice(index, index + 1);
    AppStorage.SetOrCreate("deviceListLength", this.deviceList.length);
    this.trustedDeviceModel.removeDevice(this.delDevice);
    if (this.deviceListLength == 0) {
      this.isCheck = false;
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
      Scroll(this.Scroll) {
        Column() {
          Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
            Image($r("app.media.back"))
              .width(constants.BACK_WIDTH)
              .height(constants.BACK_HEIGHT)
              .onClick(() => {
                router.push({
                  uri: 'pages/entryView'
                })
              })
            Text($r("app.string.face_introdution_title"))
              .fontSize(constants.OPTION_FONT_SIZE)
              .margin({ left: constants.PADDING_30 })
          }
          .height(constants.TITLE_HEIGHT)
          .padding({ left: constants.PADDING, right: constants.PADDING })

          Image($r("app.media.pad"))
            .width(constants.PHONE_WIDTH)
            .height(constants.PHONE_HEIGHT)
            .objectFit(ImageFit.Contain)
            .margin({ top: constants.PHONE_MARGIN_TOP, bottom: constants.PHONE_MARGIN_BOTTOM })

          Text($r("app.string.face_introdution_tip"))
            .fontSize(constants.MIN_FONT_SIZE)
            .fontColor(constants.GREY_FONT_COLOR)
            .padding({ left: constants.PADDING, right: constants.PADDING })
            .width(constants.MAX_WIDTH)
            .textAlign(TextAlign.Start)
            .margin({ top: constants.TEXT_MARGIN_TOP, bottom: constants.TEXT_MARGIN_BOTTOM })

          Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
            Text($r('app.string.face_introdution_title'))
              .fontSize(constants.TEXT_FONT_SIZE)
            Toggle({ type: ToggleType.Switch, isOn: this.isCheck })
              .width(constants.TOGGLE_SIZE)
              .onChange((isOn: boolean) => {
                this.trustedDeviceModel.getDeviceList(this.getDeviceList.bind(this));
                this.isCheck = !this.isCheck;
              })
          }
          .borderRadius(constants.ITEM_BORDER_RADIO)
          .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
          .margin({ top: constants.PADDING_30 })
          .padding({ left: constants.PADDING, right: constants.PADDING })
          .height(constants.TITLE_HEIGHT)

          if (this.isCheck) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text($r("app.string.trusted_devices_title"))
                .width(constants.MAX_WIDTH)
                .padding({ left: constants.PADDING, right: constants.PADDING })
                .fontSize(constants.MIN_FONT_SIZE)
                .fontColor(constants.GREY_FONT_COLOR)
                .textAlign(TextAlign.Start)
                .margin({
                  top: constants.TRUSTED_TEXT_MARGIN_TOP,
                  bottom: constants.TRUSTED_TEXT_MARGIN_BOTTOM,
                })
              if (this.ifCallback) {
                List({ space: constants.LIST_SPACE }) {
                  ForEach(this.deviceList, (item, index) => {
                    ListItem() {
                      Flex({
                        direction: FlexDirection.Row,
                        alignItems: ItemAlign.Center,
                        justifyContent: FlexAlign.SpaceBetween,
                      }) {
                        Flex({
                          direction: FlexDirection.Column,
                          justifyContent: FlexAlign.Center,
                          alignItems: ItemAlign.Start
                        }) {
                          Text(item.name)
                            .fontSize(constants.TEXT_FONT_SIZE)
                            .margin({ bottom: constants.PADDING_4 })
                          Text(item.isLock ? $r("app.string.connect_tip") : $r("app.string.disconnect_tip"))
                            .fontSize(constants.MIN_FONT_SIZE)
                            .fontColor(constants.GREY_FONT_COLOR)
                        }

                        Image($r("app.media.into_arrow"))
                          .width(constants.INTO_ARROW_WIDTH)
                          .height(constants.INTO_ARROW_HEIGHT)
                          .objectFit(ImageFit.Contain)
                      }
                      .padding({ left: constants.PADDING, right: constants.PADDING })
                      .onClick(() => {
                        this.delDevice = item;
                        this.removeDeviceDialogController.open();
                      })
                      .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
                      .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
                      .borderRadius(constants.ITEM_BORDER_RADIO)
                      .height(constants.LIST_ITEM_HEIGHT)
                    }.editable(false)
                  })
                }
                .width(constants.MAX_WIDTH)
                .listDirection(Axis.Vertical)
                .editMode(false)
                .edgeEffect(EdgeEffect.Spring)
              }
              Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
                Text($r('app.string.face_introdution_button'))
                  .fontSize(constants.TEXT_FONT_SIZE)
                  .fontColor(constants.BLUE_FONT_COLOR)
              }
              .borderRadius(constants.ITEM_BORDER_RADIO)
              .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
              .padding({ left: constants.PADDING, right: constants.PADDING })
              .height(constants.TITLE_HEIGHT)
              .margin({ top: constants.PADDING })
              .onClick(() => {
                this.addDeviceDialogController.open();
              })
            }
            .width(constants.MAX_WIDTH)
          }
        }
        .padding({
          left: constants.PADDING,
          right: constants.PADDING
        })
      }
      .scrollBarWidth(constants.SCROLLBAR_WIDTH)
    }
    .width(constants.MAX_WIDTH)
    .height(constants.MAX_HEIGHT)
    .backgroundColor(constants.GREY_BACKGROUND_COLOR)
  }
}