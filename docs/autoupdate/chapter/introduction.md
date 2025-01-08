# 概述

- [The best update frameworks for windows](https://omaha-consulting.com/best-update-framework-for-windows-10-7/)

# 流程

- **打包 `package`** : 将应用程序生成特定的压缩包，方便更新下载与安装
  - **全量包** ： 包含完整的应用程序
  - **delta 包** ： 只包含应用程序被修改的部分
- **发布 `distribute`** : 将打好的包放到包管理服务上，用户可以从服务上下载程序包
  - **通道 `channel`**： 可以把软件划分为 `alpha`、`beta`、`stable` 等大类，然后分别维护更新
- **部署/安装 `install`**：在开发计算机以外的计算机上安装应用程序
- **更新 `update`** ：软件已经计算机上安装成功，需要下载新的版本
  - **全量更新** ：删除本地上已经存在的程序，下载最新的安装包并安装
  - **delta 更新** : 只下载 delta 包，就能实现程序更新

# 框架

## Omaha

[Omaha](https://github.com/google/omaha/) 基于 [wix](https://wixtoolset.org/docs/wix3/) 打包器，由谷歌开发的自动更新框架，用来管理谷歌 `windows` 应用程序的自动升级，例如 `chrome`。在 `Windows` 自动更新框架中最成熟、稳定，但是部署配置及其复杂，适合大型的多用户项目。

## Squirrel

[Squirrel](https://github.com/Squirrel/Squirrel.Windows) 是比 Omaha 更轻量化的自动更新框架，基于 [NuGet](https://learn.microsoft.com/zh-cn/nuget/what-is-nuget) 包管理器，通过一系列的工具和动态库实现 Windows 应用程序的安装和更新。目前 `Electron` 使用 Squirrel 管理软件更新。



## wyBuild

[wyBuild](https://wyday.com/wybuild/) 框架主要服务于 `.NET` 程序，通过 wyBuild 工具进行打包，然后使用 wyUpdate 实现更新。**但是收费**。


## velopack

[velopack](https://github.com/velopack/velopack) 是最新出厂且最为通用的自动更新框架，基于 `NuGet` 包管理器实现，支持 `windows`、`linux` 、`OSX`平台，且适用于 `c#`、`c/c++`、`js`、`rust` 语言开发的应用程序。

>[!note]
> `velopack` 自己实现了打包工具 `vpk` ，并不是 valve 公司开发的 `vpk` 工具。

# 打包部署

## clickonce

[clickonce](https://learn.microsoft.com/zh-cn/cpp/windows/clickonce-deployment-for-visual-cpp-applications?view=msvc-170) : 一种部署技术，它允许创建基于 Windows 的自更新应用程序，这些应用程序可以通过最低程度的用户交互来安装和运行。核心体系结构基于两个 XML 清单文件：应用程序清单和部署清单，用于描述从哪里安装 ClickOnce 应用程序、如何更新这些应用程序以及何时更新它们。

## Windows Installer

[Windows Installer](https://learn.microsoft.com/zh-cn/windows/win32/Msi/windows-installer-portal) 微软提供的软件管理系统，管理软件的安装、软件组件的添加和删除、监视文件复原、并通过使用回滚来维护基本的灾难恢复。此外，Windows Installer还支持从多个源安装和运行软件，并且可以由要安装自定义程序的开发人员定制。

## WIX

[Wix tool](https://wixtoolset.org/docs/wix3/) 是基于 `Windows Installer` 的打包工具，用于将开发好的软件或者三方库打包成Windows安装程序（Microsoft Installer，MSI）。这个工具本身是开源的，通过XML文件来配置打包属性。

## NuGet

[NuGet](https://learn.microsoft.com/zh-cn/nuget/what-is-nuget) : 创建与管理以 `.nupkg` 为后缀的 ZIP 格式的拓展包，包含编译代码 (Dll)、相关文件以及描述性清单（包含包版本号等信息）。

## vpk valve

[vpk](https://developer.valvesoftware.com/wiki/VPK) 打包工具是一款用于 Steam 游戏的文件打包工具，将游戏的资源文件打包成一个 `.vpk` 文件，方便游戏开发者进行管理和发布。其工作原理同 `NuGet`，`.vpk` 文件类似于 `.zip` 文件的压缩文件，包含了游戏资源文件的路径和文件名等信息。


# 发布更新

## WinSparkle

[WinSparkle](https://github.com/vslavik/winsparkle) 是 MAC 系统自动更新框架 [Sparkle](https://sparkle-project.org/) 框架的仿造版本，使用  `Appcast` 配置文件维护版本。其功能十分简单只实现了从服务下载安装包的能力，未集成任何部署能力，此外对 Sparkle 的功能也未完全支持。

## AutoUpdater

[AutoUpdater.NET](https://github.com/ravibpatel/AutoUpdater.NET) 服务于  `.NET` 应用程序更新，工作原理同 `WinSparkle`，通过一个 `.xml` 配置文件管理软件版本信息。

## wyUpdate

[wyUpdate](https://github.com/wyday/wyupdate) 是从 [wyBuild](https://wyday.com/wybuild/) 工具独立出来的软件自动更新工具。`.NET` 程序可以直接集成 [AutomaticUpdater](https://github.com/wyday/automatic-updater) 模块，获得 wyUpdate 自动更新能力。然而，对于非 `.NET` 程序则只能调用 `wyUpdate.exe` 程序，通过命令行的形式实现实现更新检测与自动更新，例如 `wyUpdate.exe /quickcheck`。




