# velopack

# 简介

[velopack](https://github.com/velopack/velopack) 是最新出厂且最为通用的自动更新框架，基于 `NuGet` 包管理器实现，支持 `windows`、`linux` 、`OSX`平台，且适用于 `c#`、`c/c++`、`js`、`rust` 语言开发的应用程序。


# vpk 安装

>[!note]
> `velopack` 自己实现了打包工具 `vpk` ，并不是 valve 公司开发的 `vpk` 工具。


1. 下载 [.net 8.0](https://dotnet.microsoft.com/zh-cn/download/dotnet/8.0) 二进制压缩包
2. 解压工具后放入环境变量 `PATH`
3. 使用 `dotnet` 下载安装包

    ```term
    triangle@LEARN:~$ dotnet tool install --global vpk // 安装到默认全局路径 %USERPROFILE%\.dotnet\tools
    triangle@LEARN:~$ dotnet tool install --tool-path ./vpk vpk // 安装到本地目录
    ```


# 使用

1. 下载[动态库](https://github.com/velopack/velopack/releases/tag/0.0.1053)，**需要将要使用的动态库名改成 `velopack_libc.dll`**

2. 根据 [教程](https://docs.velopack.io/category/quick-start) 在代码中集成 `velopack`，编译得到目标程序

    ```cpp
    #include "Velopack.h"

    // 入口定义
    int main(int argc**, char *argv[ ])
    {
        // This should run as early as possible in the main method.
        // Velopack may exit / restart the app at this point. 
        // See VelopackApp class for more options/configuration.
        Velopack::VelopackApp::Build().Run();

    }

    // 更新检测
    static void update_app()
    {
        // https://localhost:8080/Release/ 为 vpk 生成的发布工程所在地址
        Velopack::UpdateManager manager("https://localhost:8080/Release/");

        auto updInfo = manager.CheckForUpdates();
        if (!updInfo.has_value()) {
            return; // no updates available
        }

        // download the update, optionally providing progress callbacks
        manager.DownloadUpdates(updInfo.value());

        // prepare the Updater in a new process, and wait 60 seconds for this process to exit
        manager.WaitExitThenApplyUpdate(updInfo.value());
        exit(0); // exit the app to apply the update
    }
    ```

3. 程序打包，生成发布工程
    
    ```term
    triangle@LEARN:~$ vpk -h
    Description:
    Velopack CLI 0.0.1053, for distributing applications.

    Usage:
    vpk [command] [options]

    Options:
    -h, -H, --help, --vhelp  Show help (-h) or extended help (-H).
    -x, --legacyConsole      Disable console colors and interactive components.
    -y, --yes                'yes' by instead of 'no' in non-interactive prompts.
    --verbose                Print diagnostic messages.
    --skip-updates           Skip update checks

    Commands:
    pack      Creates a release from a folder containing application files.
    download  Download's the latest release from a remote update source.
    upload    Upload local package(s) to a remote update source.
    delta     Utilities for creating or applying delta packages.
    triangle@LEARN:~$ vpk pack -u <project_name> -v 1.0.0 -p <dir> -e <main exe>
    triangle@LEARN:~$ tree .
    .
    └── Releases                        # 发布文件夹
        ├── RELEASES                    # 全量包记录
        ├── assets.win.json             # 包类型记录
        ├── releases.win.json           # 包更新信息记录
        ├── demo-1.0.0-full.nupkg       # 全量包
        ├── demo-1.0.1-delta.nupkg      # delta 包
        ├── demo-1.0.1-full.nupkg
        ├── demo-win-Portable.zip
        └── demo-win-Setup.exe          # 安装器
    ```
    
4. 将发布文件夹 `Releases` 部署到服务上，且能让客户端访问该文件夹 `https://localhost:8080/Release/`
