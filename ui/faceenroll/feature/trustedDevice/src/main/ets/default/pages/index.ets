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
        .fontSize(constants.TITLE_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TITLE_MARGIN_BOTTOM })
      Text($r('app.string.add_device_tip'))
        .fontSize(constants.TEXT_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TEXT_MARGIN_BOTTOM })
      Text($r("app.string.add_device_or_not_tip"))
        .fontSize(constants.TEXT_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TEXT_MARGIN_BOTTOM })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Text($r('app.string.cancel_button'))
          .fontColor(Color.Blue)
          .fontSize(constants.OPTION_FONT_SIZE)
          .onClick(() => {
            this.controller.close();
            this.cancel();
          })
        Divider()
          .vertical(true)
          .strokeWidth(constants.DIALOG_DRIVER_STROKE_WIDTH)
          .color(constants.GREY_BACKGROUND_COLOR)
          .height(constants.DIALOG_DRIVER_HEIGHT)
        Text($r('app.string.add_device_button'))
          .fontColor(Color.Blue)
          .fontSize(constants.OPTION_FONT_SIZE)
          .onClick(() => {
            this.controller.close();
            this.confirm();
          })
      }
    }
    .width(constants.ITEM_WIDTH)
    .borderRadius(constants.ITEM_BORDER_RADIO)
    .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      bottom: constants.PADDING_30,
      top: constants.DIALOG_TITLE_MARGIN_TOP,
      left: constants.PADDING_30,
      right: constants.PADDING_30
    })
  }
}

@CustomDialog
struct addDeviceDialog {
  private trustedDeviceModel: trustedModel;
  useTipDialogController: CustomDialogController = new CustomDialogController({
    builder: useTipDialog({ confirm: this.isAddDevice.bind(this) }),
    autoCancel: false,
    alignment: DialogAlignment.Bottom
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
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Text($r("app.string.face_introdution_button"))
        .fontSize(constants.TITLE_FONT_SIZE)
        .textAlign(TextAlign.Start)
        .width(constants.ITEM_WIDTH)
        .margin({ bottom: constants.DIALOG_TEXT_MARGIN_BOTTOM })


      List() {
        ForEach(this.addDeviceList, (item) => {
          ListItem() {
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Start }) {
                Text(item.name)
                  .fontSize(constants.TEXT_FONT_SIZE)
                  .fontColor(item.isLock ? constants.BLACK_FONT_COLOR : constants.GREY_FONT_COLOR)
                  .margin({ bottom: constants.PADDING_4 })
                Text(item.isLock ? $r("app.string.connect_tip") : $r("app.string.disconnect_tip"))
                  .fontColor(item.isLock ? constants.BLACK_FONT_COLOR : constants.GREY_FONT_COLOR)
                  .fontSize(constants.MIN_FONT_SIZE)
              }

              Image($r("app.media.into_arrow"))
                .width(constants.INTO_ARROW_WIDTH)
                .objectFit(ImageFit.Contain)
            }
            .height(constants.CONTENT_TIPS_HEIGHT)

          }.editable(false)
          .onClick(() => {
            if (item.isLock) {
              this.selectDevice = item;
              this.controller.close();
              this.useTipDialogController.open();
            }
          })
        })
      }
      .width(constants.ITEM_WIDTH)
      .divider({ color: constants.GREY_BACKGROUND_COLOR, strokeWidth: constants.DIALOG_DRIVER_STROKE_WIDTH })
    }
    .margin({ top: constants.DIALOG_TITLE_MARGIN_TOP, left: constants.PADDING_30, right: constants.PADDING_30 })
    .constraintSize({ maxHeight: constants.ADD_DIALOG_HEIGHT })
  }
}


@CustomDialog
struct removeDeviceDialog {
  controller: CustomDialogController;
  cancel: () => void;
  confirm: () => void;
//@Prop device:string

  build() {
    Column() {
      Text($r('app.string.remove_device_title'))
        .fontSize(constants.TITLE_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TITLE_MARGIN_BOTTOM })
      Text($r('app.string.remove_device_tip'))
        .fontSize(constants.TEXT_FONT_SIZE)
        .margin({ bottom: constants.DIALOG_TEXT_MARGIN_BOTTOM })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Text($r('app.string.cancel_button'))
          .fontColor(Color.Blue)
          .fontSize(constants.OPTION_FONT_SIZE)
          .onClick(() => {
            this.controller.close();
            this.cancel();
          })
        Divider()
          .vertical(true)
          .strokeWidth(constants.DIALOG_DRIVER_STROKE_WIDTH)
          .color(constants.GREY_BACKGROUND_COLOR)
          .height(constants.DIALOG_DRIVER_HEIGHT)
        Text($r('app.string.remove_device_button'))
          .fontColor(Color.Red)
          .fontSize(constants.OPTION_FONT_SIZE)
          .onClick(() => {
            this.controller.close();
            this.confirm();
          })
      }
    }
    .width(constants.ITEM_WIDTH)
    .borderRadius(constants.ITEM_BORDER_RADIO)
    .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
    .alignItems(HorizontalAlign.Start)
    .padding({
      bottom: constants.PADDING_30,
      top: constants.DIALOG_TITLE_MARGIN_TOP,
      left: constants.PADDING_30,
      right: constants.PADDING_30
    })
  }
}

@Component
export default struct trustedDevice {
  private trustedDeviceModel: trustedModel;
  private userAuthModel: userAuthModel;
  @State isUpdate: boolean = false;
  @State ifCallback: boolean = false;
  Scroll: Scroller = new Scroller();
  @State isCheck: boolean = false;
  @State isBlueTooth: boolean = true;
  @State deviceList: [{
    name: string,
    isLock: boolean
  }]= [{ name: 'test', isLock: true }];
  @StorageLink('deviceListLength') deviceListLength: number = 1;
  @State addDevice: {
    name: string,
    isLock: boolean
  } = { name: "", isLock: true }
  delDevice: {
    name: string,
    isLock: boolean
  } = { name: "", isLock: true }

  removeDeviceDialogController: CustomDialogController = new CustomDialogController({
    builder: removeDeviceDialog({ cancel: this.onCancel, confirm: this.removeDevice.bind(this) }),
    cancel: this.onCancel,
    autoCancel: false,
    alignment: DialogAlignment.Bottom
  })

  addDeviceDialogController: CustomDialogController = new CustomDialogController({
    builder: addDeviceDialog({ addDevice: $addDevice, confirm: this.isAddDevice.bind(this) }),
    cancel: this.onCancel,
    autoCancel: true,
    alignment: DialogAlignment.Bottom
  })

  aboutToAppear() {
    this.trustedDeviceModel = trustedModel.getInstance();
    this.userAuthModel = userAuthModel.getInstance();
    this.userAuthModel.getIsCheck(this.getIsCheck.bind(this));
  }

  getIsCheck(isCheck) {
    this.isCheck = isCheck
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
    AppStorage.SetOrCreate("deviceListLength", this.deviceList.length);
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
          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
            Image($r("app.media.back"))
              .width(constants.BACK_WIDTH)
              .height(constants.MAX_HEIGHT)
              .padding({ left: constants.BACK_PADDING, right: constants.BACK_PADDING })
              .onClick(() => {
                router.push({
                  uri: 'pages/entryView'
                })
              })
            Text($r("app.string.face_introdution_title"))
              .fontSize(constants.TEXT_FONT_SIZE)
              .width(constants.TITLE_TEXT_WIDTH)
          }
          .width(constants.ITEM_WIDTH)
          .height(constants.TITLE_HEIGHT)

          Image($r("app.media.phone"))
            .width(constants.PHONE_WIDTH)
            .height(constants.PHONE_HEIGHT)
            .objectFit(ImageFit.Contain)
            .margin({ top: constants.PHONE_MARGIN_TOP, bottom: constants.PHONE_MARGIN_BOTTOM })

          Text($r("app.string.face_introdution_tip"))
            .width(constants.CONTENT_TIPS_WIDTH)
            .height(constants.CONTENT_TIPS_HEIGHT)
            .fontSize(constants.MIN_FONT_SIZE)
            .fontColor(constants.GREY_FONT_COLOR)
            .textAlign(TextAlign.Center)
            .margin({
              top: constants.ADD_TEXT_MARGIN_TOP,
              bottom: constants.ADD_TEXT_MARGIN_TOP,
            })
          Flex({
            direction: FlexDirection.Row,
            alignItems: ItemAlign.Center,
            justifyContent: FlexAlign.SpaceBetween
          }) {
            Text($r("app.string.face_introdution_title"))

              .fontSize(constants.TEXT_FONT_SIZE)
            Toggle({ type: ToggleType.Switch, isOn: this.isCheck })
              .onChange((isOn: boolean) => {
                this.trustedDeviceModel.getDeviceList(this.getDeviceList.bind(this));
                this.isCheck = !this.isCheck;
              })
          }
          .borderRadius(constants.ITEM_BORDER_RADIO)
          .width(constants.ITEM_WIDTH)
          .height(constants.CONTENT_TIPS_HEIGHT)
          .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
          .padding({ left: constants.PADDING, right: constants.PADDING })


          if (this.isCheck) {
            Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
              Text($r("app.string.trusted_devices_title"))
                .padding({ left: constants.PADDING, right: constants.PADDING })
                .width(constants.MAX_WIDTH)
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
                      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
                        Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
                          Text(item.name)
                            .width(constants.MAX_WIDTH)
                            .fontSize(constants.TEXT_FONT_SIZE)
                            .height(constants.LIST_TEXT_HEIGHT)
                            .textAlign(TextAlign.Start)
                          Text(item.isLock ? $r("app.string.connect_tip") : $r("app.string.disconnect_tip"))
                            .width(constants.MAX_WIDTH)
                            .fontSize(constants.MIN_FONT_SIZE)
                            .height(constants.LIST_TEXT_HEIGHT)
                            .fontColor(constants.GREY_FONT_COLOR)
                            .textAlign(TextAlign.Start)
                        }
                        .width(constants.ITEM_WIDTH)
                        .height(constants.MAX_HEIGHT)

                        Image($r("app.media.into_arrow"))
                          .width(constants.INTO_ARROW_WIDTH)
                          .height(constants.INTO_ARROW_HEIGHT)
                          .objectFit(ImageFit.Contain)
                      }
                      .onClick(() => {
                        this.delDevice = item
                        this.removeDeviceDialogController.open()
                      })
                      .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
                      .padding({
                        top: constants.LIST_ITEM_MARGIN_TOP,
                        bottom: constants.LIST_ITEM_MARGIN_BOTTOM,
                        left: constants.LIST_ITEM_MARGIN_LEFT,
                        right: constants.LIST_ITEM_MARGIN_LEFT
                      })
                      .borderRadius(constants.ITEM_BORDER_RADIO)
                      .width(constants.MAX_WIDTH)
                      .height(constants.LIST_ITEM_HEIGHT)
                    }.editable(false)
                    .width(constants.MAX_WIDTH)
                  })
                }
                .width(constants.ITEM_WIDTH)
                .listDirection(Axis.Vertical)
                .editMode(false)
                .edgeEffect(EdgeEffect.Spring)
              }
              Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
                Text($r("app.string.face_introdution_button"))
                  .fontSize(constants.TEXT_FONT_SIZE)
                  .fontColor(constants.BLUE_FONT_COLOR)
                  .onClick(() => {
                    this.addDeviceDialogController.open()
                  })
                  .height(constants.LIST_ITEM_HEIGHT)
              }
              .margin({
                top: constants.ADD_TEXT_MARGIN_TOP,
                bottom: constants.ADD_TEXT_MARGIN_BOTTOM,
              })
              .padding({ left: constants.PADDING, right: constants.PADDING })
              .borderRadius(constants.ITEM_BORDER_RADIO)
              .width(constants.ITEM_WIDTH)
              .height(constants.UNLOCK_TEXT_HEIGHT)
              .backgroundColor(constants.WHITE_BACKGROUND_COLOR)
            }
            .width(constants.MAX_WIDTH)
          }
        }
      }
      .scrollBarWidth(constants.SCROLLBAR_WIDTH)
    }
    .width(constants.MAX_WIDTH)
    .height(constants.MAX_HEIGHT)
    .backgroundColor(constants.GREY_BACKGROUND_COLOR)
  }
}




