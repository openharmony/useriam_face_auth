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
export default class Config {
  public static FACE_TOKEN: array = new Uint8Array([1,2,3,4,5,6])
  public static TRUSTED_DEVICE_CHECK = false
  public static AUTH_TYPE = 2
  public static TYPE2D = "2D"
  public static AUTH = [
    {
      credentialId:new Uint8Array([1,2,3,4,5,6]),
      authType:2,
      authSubType:20000,
      templateId:1
    },
    {
      credentialId:new Uint8Array([1,2,3,4,5,6]),
      authType:2,
      authSubType:20000,
      templateId:2
    }
  ]
  public static FACE_DATA  = {
    credentialId: credentialId,
    authType: 2,
    authSubType: 1,
    templateId: 1
  }
  public static DEVICE_LIST = [
    {
      name: "HuaWei Mate40",
      isLock: true
    },
    {
      name: "HuaWei Mate30",
      isLock: true
    },
    {
      name: "HuaWei p40",
      isLock: true
    },
    {
      name: "HuaWei p30",
      isLock: true
    }
  ]
  public static ADD_LIST = [
    {
      name: "HuaWei Mate40 add",
      isLock: true
    },
    {
      name: "HuaWei Mate30 add",
      isLock: true
    },
    {
      name: "HuaWei p40 add",
      isLock: false
    },
    {
      name: "HuaWei p30 add",
      isLock: false
    }
  ]
}