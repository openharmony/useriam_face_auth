# useriam_faceauth

#### 介绍
useriam_faceauth是用户身份认证与访问控制子系统的人脸认证组件。提供了人脸认证相关接口（如人脸录入、人脸认证、人脸删除等）和人脸认证UX界面（如设置页面、录入页面等）。

#### 目录
```bash
//base/useriam/faceauth
├── BUILD.gn                 # 组件构建脚本
├── common                   # 组件公共文件
├── interfaces
│   ├── innerkit             # 内部接口，供系统应用使用
│   └── kits                 # 开放接口，供应用开发者使用
├── ohos.build               # 组件描述文件
├── sa_profile               # SA 配置文件
├── services                 # SA 服务实现
├── useriam.gni              # 构建配置
└── ux                       # UX界面
```


#### 编译构建

1. 构建环境搭建

   请参考：[https://www.openharmony.cn/pages/00010103/](https://gitee.com/link?target=https%3A%2F%2Fwww.openharmony.cn%2Fpages%2F00010103%2F)

2. 源码获取方式

   请参考：[https://www.openharmony.cn/pages/00010103/](https://gitee.com/link?target=https%3A%2F%2Fwww.openharmony.cn%2Fpages%2F00010103%2F)

3. 源码构建

   以产品Hi3516DV300为例，参考文档：[https://www.openharmony.cn/pages/00010103/](https://gitee.com/link?target=https%3A%2F%2Fwww.openharmony.cn%2Fpages%2F00010103%2F)

   全量构建：

   ```bash
   ./build.sh --product-name Hi3516DV300
   ```

   人脸认证组件独立构建：

   ```bash
   ./build.sh --product-name Hi3516DV300 --build-target faceauth   # 说明：faceauth 是组件名称，非子系统名称
   ```

   人脸认证ST测试例构建：

   ```bash
   ./build.sh --product-name Hi3516DV300 --build-target faceauth_test
   ```

   人脸认证UT测试例构建：

   ```bash
   ./build.sh --product-name Hi3516DV300 --build-target faceauth_build_module_standard_test
   ```

   人脸认证UX构建：

   在华为鸿蒙应用开发官网下载DevEco Studio 3.0 Beta1及以上版本并安装。安装和UX构建步骤请参考：https://developer.harmonyos.com/cn/docs/documentation/doc-guides/tools_overview-0000001053582387

#### 使用说明

   开发者可以通过统一身份认证Kit开放的人脸认证功能API使用相关功能。