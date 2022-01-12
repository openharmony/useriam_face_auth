# 人脸识别组件

- [人脸识别组件](#人脸识别组件)
  - [简介<a name="section11660541593"></a>](#简介)
  - [目录<a name="section161941989596"></a>](#目录)
  - [安装<a name="section14778154275818"></a>](#安装)
  - [相关仓<a name="section1371113476307"></a>](#相关仓)

## 简介<a name="section11660541593"></a>
faceauth是用户身份认证与访问控制子系统的人脸认证组件。提供了人脸认证相关接口（如人脸录入、人脸认证、人脸删除）和人脸认证UI界面（如设置页面、录入页面）。

## 目录<a name="section161941989596"></a>

```bash
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

## 安装<a name="section14778154275818"></a>

人脸识别组件随用户身份认证与访问控制子系统编译和系统镜像打包。

## 相关仓<a name="section1371113476307"></a>

distributedschedule_samgr

distributedschedule_safwk

appexecfwk_standard

graphic_standard
