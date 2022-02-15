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
import router from '@system.router';
import Constants from '../common/constant.ets';

async function routeCase() {
  let options = {
    uri: 'pages/entryView'
  }
  try {
    await router.push(options);
  } catch (err) {
    console.error(`fail callback, code: ${err.code}, msg: ${err.msg}`);
  }
}

@Entry
@Component
struct Success {

  onPageShow(){
    setTimeout(()=>{
      router.push({
        uri: 'pages/userAuth'
      })
    },Constants.TIMEOUT_2000)
  }
  private onBackPress(): void{
    AppStorage.Delete('faceToken');
    router.push({
      uri: 'pages/entryView',
    })
  }
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Flex() {
        Image($r('app.media.back'))
          .width(Constants.BACK_WIDTH)
          .height(Constants.BACK_HEIGHT)
      }.margin({ top: $r('app.float.back_margin') })
      .onClick(()=>{
        routeCase()
      })

      Text($r('app.string.face_record_success'))
        .fontSize(Constants.TITLE_FONT_SIZE)
        .fontWeight(FontWeight.Bold)
        .margin({ top: Constants.TITLE_MARGIN})

      Flex({ alignItems: ItemAlign.Center , justifyContent:FlexAlign.Center}){
        Image($r('app.media.success'))
          .width(Constants.SUCCESS_WIDTH)
          .height(Constants.SUCCESS_WIDTH)
      }.width('100%')
      .height(Constants.SUCCESS_WIDTH)
      .margin({ top: $r('app.float.success_margin') })
    }
    .width('100%')
    .height('100%')
  }
}