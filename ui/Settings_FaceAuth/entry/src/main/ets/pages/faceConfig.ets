/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

import BackComponent from '../component/backComponent';
import Log from '../utils/log';
import CommonController from '../controller/commonController';
import FaceConfigController from '../controller/faceConfigController';
import ConfirmDialog from '../component/confirmDialog';
import router from '@system.router';

@Extend(Span) function configSetSpanFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_secondary'))
  .fontSize($r('sys.float.ohos_id_text_size_caption1'))
  .fontWeight(FontWeight.Regular)
}

@Extend(Span) function configSetLinkFormat() {
  .fontColor($r('sys.color.ohos_id_color_text_hyperlink'))
  .fontSize($r('sys.float.ohos_id_text_size_caption1'))
  .fontWeight(FontWeight.Medium)
}

@Extend(Text) function configDeleteData() {
  .fontColor($r('sys.color.ohos_id_color_warning'))
  .fontSize($r('sys.float.ohos_id_text_size_body1'))
  .fontWeight(FontWeight.Medium)
}

@Entry
@Component
struct FaceConfig {
  @State readonly TAG: string = 'FaceConfig';
  @State touched: boolean = false;
  @StorageLink('CONTENT_TYPE_WIDTH') CONTENT_TYPE_WIDTH: number = 0;
  @StorageLink('CARD_TYPE_WIDTH') CARD_TYPE_WIDTH: number = 0;

  private deleteFaceDialogController: CustomDialogController = new CustomDialogController({
    builder: ConfirmDialog({
      title: $r('app.string.delete_record_title'),
      content: $r('app.string.wether_delete_face_data'),
      confirmText: $r('app.string.face_record_delete'),
      cancelText: $r('app.string.face_record_cancel'),
      confirmOperation: FaceConfigController.deleteFace
    }),
    alignment: CommonController.getDialogAlignment(),
    offset: {
      dx : '0vp',
      dy : CommonController.getDialogYOffset()
    }
  });

  build() {
    Column() {
      Column() {
        BackComponent({ prevTag: this.TAG, title: $r('app.string.face_checker') })
        Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
          Row() {
            Text($r('app.string.delete_record_title'))
              .configDeleteData()
              .margin({ left: '24vp', right: '24vp', top: '12vp', bottom: '12vp' })
              .textAlign(TextAlign.Start);
          }
        }
        .backgroundColor(this.touched ? '#D8D8D8' : '#FFFFFF')
        .height('56vp')
        .margin({
          top: '8vp',
          bottom: '8vp'
        })
        .borderRadius('24vp')
        .onClick(() => {
          this.deleteFaceDialogController.open()
        })
        .onTouch((event?: TouchEvent) => {
          if (event !== undefined) {
            if (event.type === TouchType.Down) {
              this.touched = true;
            }
            if (event.type === TouchType.Up) {
              this.touched = false;
            }
          }
        });
        Column() {
          Text() {
            Span($r('app.string.face_3dguide_info_2'))
            .configSetSpanFormat()
            Span($r('app.string.face_guide_2d_announce_link'))
            .configSetLinkFormat()
            .onClick(() => {
              router.push({ uri: 'pages/enrollDeclare' })
            })
            Span($r('app.string.period'))
            .configSetSpanFormat()
          }
          .textAlign(TextAlign.Start)
        }
        .margin({
          left: '24vp',
          right: '24vp'
        })
      }
      .alignItems(HorizontalAlign.Start)
      .width(this.CONTENT_TYPE_WIDTH)
    }
    .width('100%')
    .height('100%')
    .backgroundColor($r('sys.color.ohos_id_color_sub_background'))
    .onAreaChange((_, newArea) => {
      Log.info(this.TAG, 'new display area :' + JSON.stringify(newArea))
      CommonController.setDisplaySize(Number(newArea.width), Number(newArea.height))
      Log.info(this.TAG, 'total area w: ' + newArea.width + ' h: ' + newArea.height)
    })
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+');
    CommonController.routeBack();
    Log.info(this.TAG, 'onBackPress-');
    return true;
  }
}