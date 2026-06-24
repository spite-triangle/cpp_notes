# JDK

# 简介

`JDK（Java Development Kit）`是Java编程语言的开发工具包，它包含了Java编译器、运行时环境以及一些常用的库和工具。JDK的主要目标是为开发者提供一个完整的开发环境，使得他们可以编写、编译和运行Java程序。但也类似 `c/c++` 编译器，也存在不同的发行版本
- [OpenJDK](https://openjdk.org/): 标准协议版本，旨在用于推进 `java` 技术发展，**不建议作为生成环境**
- [Oracle JDK](https://www.oracle.com/java/technologies/javase-downloads.html): 由Oracle开发的商业版JDK
- [Temurin](https://adoptium.net/zh-CN/temurin/releases): 由 `Eclipse Adoptium` 项目构建完全免费的 OpenJDK 构建版，**生产环境推荐**

# SDK 安装

## 系统环境

从 [Termurin](https://adoptium.net/zh-CN/temurin/releases) 下载 `JDK` 压缩包，解压到指定目录，并配置环境变量

```ini
JAVA_HOME=c:/Program Files/Java/jdk-17          # jdk 目录
PATH=%JAVA_HOME%\bin                            # PATH 中添加
```

## vfox (推荐)

[vfox](https://vfox.dev/) 是一个支持 `java`、`nodejs`、`make`、`cmake` 等开发 `SDK` 的通用版本管理工具，安装相应 `SDK` 的插件后，便能实现`SDK`版本管理功能。

1. 下载 [vfox](https://github.com/version-fox/vfox)
2. vfox 的 SDK 、插件，配置默认存放目录在 `$HOME/.fox/`
   - 修改插件存储位置 `vfox.exe config storage.sdkPath <your_sdk_path>`
3. 安装管理插件 `vfox add java` , **下载不了也能离线安装，`vfox add --source java.zip  java`**
4. [ `JDK` 安装命令](https://github.com/version-fox/vfox-java) 
    ```bash
    vfox install java@17.0.17          # openJDK
    vfox install java@17.0.17-tem      # Temurin
    vfox install java@17.0.17-zulu     # Zulu
    vfox install java@17-graal         # GraalVM (latest 17.x) 
    ```
5. 修改 `$HOME/.fox/plugin/java/lib/foojay.lua` 中的 `URL` 路径，可替换下载源
6. 使用 
   - `vfox use java[@version]` 终端切换
   - `vfox use -g java[@version]` 系统默认

如果是完全断网的离线环境，可以从 `https://adoptium.net/zh-CN/temurin/releases` 下载压缩包，然后通过[vfox-install](../../code/bash/vfox-install.sh)解压到 `vfox.exe config storage.sdkPath` 目录下，目录组织结构如下

```
./
└── java
    ├── v-17+35-tem
    │   └── java-17+35-tem
    │       ├── NOTICE
    │       ├── bin
    │       ├── conf
    │       ├── include
    │       ├── jmods
    │       ├── legal
    │       ├── lib
    │       └── release
    └── v-26.0.1+1
        └── java-26.0.1+1
            ├── bin
            ├── conf
            ├── include
            ├── jmods
            ├── legal
            ├── lib
            └── release
```