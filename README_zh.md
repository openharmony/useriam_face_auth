# 人脸认证组件

- [人脸认证组件](#人脸认证组件)
  - [简介<a name="section11660541593"></a>](#简介)
  - [目录<a name="section161941989596"></a>](#目录)
-   [使用说明](#section741617511812)
    -   [准备](#section1579912573329)
    -   [获取源码](#section11443189655)
    -   [编译构建](#section2081013992812)

  - [相关仓<a name="section1371113476307"></a>](#相关仓)

## 简介<a name="section11660541593"></a>
faceauth是人脸认证组件，提供人脸认证能力，可应用于设备解锁、支付、应用登录等身份认证场景。

人脸认证组件向上对接系统应用和开发者应用，提供了人脸认证相关接口（如人脸录入、人脸认证、人脸删除）和人脸认证UI界面（如设置页面、录入页面）；向下调度厂商提供的人脸认证实现，执行人脸认证相关功能。

## 目录<a name="section161941989596"></a>

```
//base/user_iam/face_auth
├── BUILD.gn                 # 组件构建脚本
├── common                   # 组件公共文件
├── interfaces               # 接口
│   ├── innerkit             # 对内接口，供系统应用使用
│   └── kits                 # 对外接口，供应用开发者使用
├── ohos.build               # 组件描述文件
├── sa_profile               # Service Ability 配置文件
├── service                  # Service Ability 服务实现
├── useriam.gni              # 构建配置
└── ui                       # 用户界面
```

## 使用说明<a name="section741617511812"></a>

人脸识别组件是OpenHarmony人脸识别框架。开发者可以调用框架对外接口查询设备是否支持人脸认证，调用人脸认证能力。

### 准备<a name="section1579912573329"></a>

开发者需要在Linux上搭建编译环境：

-   [Ubuntu编译环境准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-env-setup-linux.md);
-   Hi3518EV300单板：参考[环境搭建](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3518-setting.md)；
-   Hi3516DV300单板：参考[环境搭建](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3516-setting.md)。

### 获取源码<a name="section11443189655"></a>

在Linux服务器上下载并解压一套源代码，源码获取方式参考[源码获取](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md)。

### 编译构建<a name="section2081013992812"></a>

开发者开发第一个应用程序可参考：

-   [helloworld for Hi3518EV300](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3518-running.md)；

-   [helloworld for Hi3516DV300](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3516-running.md)。

## 相关仓<a name="section1371113476307"></a>

用户身份认证与访问控制子系统

distributedschedule_samgr

distributedschedule_safwk

appexecfwk_standard

graphic_standard
