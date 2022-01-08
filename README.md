# useriam_faceauth

## 介绍
useriam_faceauth是用户身份认证与访问控制子系统的人脸认证组件。提供了人脸认证相关接口（如人脸录入、人脸认证、人脸删除）和人脸认证UX界面（如设置页面、录入页面）。

## 目录
```bash
//base/useriam/faceauth
├── BUILD.gn                 # 组件构建脚本
├── common                   # 组件公共文件
├── interfaces               # 接口
│   ├── innerkit             # 内部接口，供系统应用使用
│   └── kits                 # 开放接口，供应用开发者使用
├── ohos.build               # 组件描述文件
├── sa_profile               # SA 配置文件
├── services                 # SA 服务实现
├── useriam.gni              # 构建配置
└── ux                       # UX界面
```

## 编译构建

useriam_faceauth是Open Harmony标准系统的组件，安装依赖工具、获取源码、编译流程请参考Open Harmony官方构建文档。链接如下：

[搭建Windows开发环境](https://www.openharmony.cn/pages/00010101/)

[搭建Ubuntu环境-Docker方式](https://www.openharmony.cn/pages/00010102/)

[搭建Ubuntu环境-安装包方式](https://www.openharmony.cn/pages/00010103/)

在完成全量构建后，可以单独构建faceauth组件，命令如下：

```bash
# ./build.sh $platform_build_comand --build-target faceauth
# 以Hi3516DV300为例，其他产品构建命令有所不同
./build.sh --product-name Hi3516DV300 --build-target faceauth
```

## 使用说明

本模块实现了Harmony OS用户认证接口，接口说明请参考[用户认证 JSAPI参考](https://developer.harmonyos.com/cn/docs/documentation/doc-references/js-apis-useriam-userauth-0000001168311785)。

使用示例：

```javascript
import userIAM_userAuth from '@ohos.userIAM.userAuth';

export default {
    startAuth() {
        console.info("start auth");
        let tipCallback = (tip)=>{
            console.info("receive tip: errorCode(" + tip.errorCode + ") code(" + tip.tipCode + ") event(" +
                tip.tipEvent + ") info(" + tip.tipInfo + ")");
            // 此处添加提示信息显示逻辑
        };
        let auth = userIAM_userAuth.getAuthenticator();
        auth.on("tip", tipCallback);
        auth.execute("FACE_ONLY", "S2").then((code)=>{
            auth.off("tip", tipCallback);
            console.info("auth success");
            // 此处添加认证成功逻辑
        }).catch((code)=>{
            auth.off("tip", tipCallback);
            console.error("auth fail, code = " + code);
            // 此处添加认证失败逻辑
        });
    },

    checkAuthSupport() {
        console.info("start check auth support");
        let auth = userIAM_userAuth.getAuthenticator();
        let checkCode = auth.checkAvailability("FACE_ONLY", "S2");
        if (checkCode == userIAM_userAuth.CheckAvailabilityResult.SUPPORTED) {
            console.info("check auth support success");
            // 此处添加支持指定类型认证的逻辑
        } else {
            console.error("check auth support fail, code = " + checkCode);
            // 此处添加不支持指定类型认证的逻辑
        }
    },

    cancelAuth() {
        console.info("start cancel auth");
        let auth = userIAM_userAuth.getAuthenticator();
        let cancelCode = auth.cancel();
        if (cancelCode == userIAM_userAuth.Result.SUCCESS) {
            console.info("cancel auth success");
        } else {
            console.error("cancel auth fail");
        }
    }
}
```

## 测试用例

### 执行UT用例

1. 在完成全量构建后，构建人脸faceauth UT测试例

```bash
# ./build.sh $platform_build_comand --build-target faceauth_build_module_standard_test
# 以Hi3516DV300为例，其他产品构建命令有所不同
./build.sh --product-name Hi3516DV300 --build-target faceauth_build_module_standard_test
```

2. 将UT测试例的二进制推送到设备上

```bash
# 以Hi3516DV300为例，其他产品输出路径有所不同
hdc file send out/hi3516dv300/tests/unittest/faceauth/faceauthtest/face_auth_manager_auth_test /data/face_auth_manager_auth_test
hdc file send out/hi3516dv300/tests/unittest/faceauth/faceauthtest/face_auth_manager_enroll_test /data/face_auth_manager_enroll_test
hdc file send out/hi3516dv300/tests/unittest/faceauth/faceauthtest/face_auth_manager_scenario_test /data/face_auth_manager_scenario_test
```

3. 执行测试例，观察测试结果

```bash
hdc shell /data/face_auth_manager_auth_test
hdc shell /data/face_auth_manager_enroll_test
hdc shell /data/face_auth_manager_scenario_test
```

测试结果样例：

```bash
[----------] 36 tests from FaceAuthManagerTest (222079 ms total)

[----------] Global test environment tear-down
Gtest xml output finished
[==========] 36 tests from 1 test case ran. (222093 ms total)
[  PASSED  ] 36 tests.
```
