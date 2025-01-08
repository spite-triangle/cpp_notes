# wix

- [wix tutorial](https://www.firegiant.com/wix/tutorial/getting-started/)
- [Wix Toolset打包文件夹为MSI](https://www.cnblogs.com/strugt/p/13208537.html)

# 介绍

微软设计 [Windows Installer](https://learn.microsoft.com/zh-cn/windows/win32/Msi/windows-installer-portal) 软件管理系统，用于管理软件的安装、软件组件的添加和删除、监视文件复原、并通过使用回滚来维护基本的灾难恢复。但是 Windows Installer 处理流程贼复杂，因此微软又对底层接口进行二次封装，提供了 [Wix tool](https://wixtoolset.org/docs/wix3/) 简化 Windows Installer 软件打包流程。

工作流程为:
1. 创建程序打包配置文件 `.wxs`。手动编写或通过 `heat.exe` 工具生成
2. 编译 `.wxs` 配置文件生成 `.wixobj` 文件。v3 版本使用 `candle.exe` 工具，v4/v5 则可以使用 `wix`
3. 链接 `.wixobj` 文件生成安装包 `.msi`。v3 版本使用 `light.exe` 工具，v4/v5 则可以使用 `wix`
4. [可选] 创建依赖环境的安装包。 若应用程序会依赖于一些框架，可以为这些框架编写一个 `.wxs`，并配置`<Bundle></Bundle>`，得到依赖环境的安装包。若检测到用户电脑上没有这些框架，则会自动安装这些框架。

# 简单案例

对以下文件进行打包

```term
triangle@LEARN:~$ tree .
.
├── demo.exe
└── readme.md
``` 

1. 编写配置文件


```xml
<?xml version='1.0' encoding='windows-1252'?>
<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'>

    <!-- 项目信息配置
        - Name 项目名
        - Id  可以使用 https://www.iamwawa.cn/guid.html 创建
        - UpgradeCode 同上
    -->
  <Product  
        Name='Foobar 1.0' 
        Id='096bf663-e5e4-4987-ad59-e5766d113ec3' 
        UpgradeCode='096bf663-e5e4-4987-ad59-e5766d113ec3'
        Language='1033' Codepage='1252' Version='1.0.0' Manufacturer='Acme Ltd.'>
	
    <!-- 设置MSI安装包信息
        - Id   通过 * 可以自动生成 GUID，只对 package 有效
    -->
    <Package 
        Id='*' 
        Keywords='Installer' 
        Description="Acme's Foobar 1.0 Installer"
        Comments='Foobar is a registered trademark of Acme Ltd.' Manufacturer='Acme Ltd.'
        InstallerVersion='100' Languages='1033' Compressed='yes' SummaryCodepage='1252' />
	  
    <!-- 指定从哪里获取安装包进行安装
        - EmbedCab  将资源放入MSI中，也可以将资源与MSI安装包分割
        - Cabinet   文件数据打包后的一个压缩包
        - DiskPrompt 安装磁盘，可以指定分盘
    -->
    <Media Id='1' Cabinet='Sample.cab' EmbedCab='yes' DiskPrompt="CD-ROM #1" />
    <Property Id='DiskPrompt' Value="Acme's Foobar 1.0 Installation [1]" />
	
    <!-- 定义 Sample.cab 文件中的目录层级，常量标识 https://msdn.microsoft.com/en-us/library/aa372057.aspx
        - TARGETDIR     根目录标识，规定
        - ProgramFilesFolder    系统目录标识，代表绝对路径
        - ProgramMenuFolder     系统目录标识，代表绝对路径
        - DesktopFolder         桌面目录标识，代表绝对路径
        - Name  实际的文件夹名
        - Id    唯一标识，在配置文件中使用
     -->
    <Directory Id='TARGETDIR' Name='SourceDir'>

      <!-- 定义文件目录, C:\Program Files (x86)\Acme\Foobar 1.0\ -->
      <Directory Id='ProgramFilesFolder' Name='PFiles'>
        <Directory Id='Acme' Name='Acme'>

          <!-- INSTALLDIR  指代实际的安装路径 -->
          <Directory Id='INSTALLDIR' Name='Foobar 1.0'>

            <!-- Component 定义被安装内容的最小单位 -->
            <Component Id='MainExecutable' Guid='636d41d9-d950-4a82-90f7-e186d5ebcf24'>
              <File Id='demoEXE' Name='demo.exe' DiskId='1' Source='demo.exe' KeyPath='yes'>
                <!-- win 开始菜单创建快捷方式 -->
                <Shortcut Id="startmenuFoobar10" Directory="ProgramMenuDir" Name="Foobar 1.0" WorkingDirectory='INSTALLDIR' Icon="demo.exe" IconIndex="0" Advertise="yes" />
                <!-- 在桌面创建快捷方式 -->
                <Shortcut Id="desktopFoobar10" Directory="DesktopFolder" Name="Foobar 1.0" WorkingDirectory='INSTALLDIR' Icon="demo.exe" IconIndex="0" Advertise="yes" />
              </File>
            </Component>
            
            <!-- 帮助相关文件 -->
            <Component Id='Manual' Guid='4e6393c0-e202-48ef-b87f-c89dab205350'>
              <File Id='readme' Name='readme.md' DiskId='1' Source='readme.md' KeyPath='yes' />
            </Component>
			
          </Directory>
        </Directory>
      </Directory>
	  
      <!-- win 开始菜单目录 -->
      <Directory Id="ProgramMenuFolder" Name="Programs">
        <Directory Id="ProgramMenuDir" Name="Foobar 1.0">
          <Component Id="ProgramMenuDir" Guid="47a74487-0455-4084-926c-1ffe09958a01">
            <RemoveFolder Id='ProgramMenuDir' On='uninstall' />
            <RegistryValue Root='HKCU' Key='Software\[Manufacturer]\[ProductName]' Type='string' Value='' KeyPath='yes' />
          </Component>
        </Directory>
      </Directory>
      
      <!-- 桌面目录 -->
      <Directory Id="DesktopFolder" Name="Desktop" />
    </Directory>
	
    <!-- 控制上述文件定义如何安装，如 完整，典型，自定义 安装 -->
    <Feature Id='Complete' Title='Foobar 1.0' Description='The complete package.'
      Display='expand' Level='1' ConfigurableDirectory='INSTALLDIR'>

      <!-- 安装主程序 -->
      <Feature Id='MainProgram' Title='Program' Description='The main executable.' Level='1'>
        <ComponentRef Id='MainExecutable' />
        <ComponentRef Id='ProgramMenuDir' />
      </Feature>
      
      <!-- 安装文档 -->
      <Feature Id='Documentation' Title='Description' Description='The instruction manual.' Level='1000'>
        <ComponentRef Id='Manual' />
      </Feature>
    </Feature>
	
    <!-- 指定安装器 UI： WixUI_InstallDir  -->
    <Property Id="WIXUI_INSTALLDIR" Value="INSTALLDIR" />
    <!-- 不同的UI参考 https://www.firegiant.com/wix/tutorial/user-interface/ui-wizardry/ -->
    <UIRef Id="WixUI_InstallDir" />
    <UIRef Id="WixUI_ErrorProgressText" />
    <Icon Id="demo.exe" SourceFile="demo.exe" />
	
    <!-- 安装完成后可以自定义一些动作，比如打开软件等 -->
    <!-- 参考https://www.firegiant.com/wix/tutorial/events-and-actions/extra-actions/ -->
  </Product>
</Wix>

```

编译配置文件

```term
triangle@LEARN:~$ candle.exe .\demo.wxs
triangle@LEARN:~$ light.exe .\demo.wixobj -ext WixUIExtension // WixUIExtension 指定使用 WixUI_InstallDir 安装界面
```


# 生成配置

在大型项目中手动写 `.wxs` 太丧心病狂，可以使用 `heat.exe` 根据实际文件目录创建 `.wxs` 配置文件中的 `Component` 配置。

```term
triangle@LEARN:~$ heat.exe dir <dir_path> -cg <group_name> -gg -sfrag -template fragment -out <out.wxs>
```

```xml
<?xml version="1.0" encoding="utf-8"?>
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">
  <Fragment>
      <DirectoryRef Id="TARGETDIR">
          <Directory Id="dirCFD54C07EFB094CAD317543172D62CC3" Name="bin">
              <Component Id="cmp06A30E27D9903457396064AFC9CF112F" Guid="{C7380132-F932-4028-8E17-24CC563931A2}">
                  <File Id="filBEC2E0FE0BD7DFA9BA14C6438EBB5189" KeyPath="yes" Source="demo.exe" />
              </Component>
              <Component Id="cmpF6BD60E46FB06A48D7DADAAB1BA790EE" Guid="{DDB86648-6325-451B-BCEF-5FCA3E641459}">
                  <File Id="filDF56FC3F88D48EC87C1A314AC10051FC" KeyPath="yes" Source="readme.md" />
              </Component>
          </Directory>
      </DirectoryRef>
  </Fragment>
  <Fragment>
      <ComponentGroup Id="HeatGroup">
          <ComponentRef Id="cmp06A30E27D9903457396064AFC9CF112F" />
          <ComponentRef Id="cmpF6BD60E46FB06A48D7DADAAB1BA790EE" />
      </ComponentGroup>
  </Fragment>
</Wix>
```

将生成的配置信息合并到主 `.wxs` 中

```xml
<?xml version='1.0' encoding='windows-1252'?>
<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'>

    <!-- .... -->
    <Directory Id='TARGETDIR' Name='SourceDir'>

      <!-- 定义文件目录, C:\Program Files (x86)\Acme\Foobar 1.0\ -->
      <Directory Id='ProgramFilesFolder' Name='PFiles'>
        <Directory Id='Acme' Name='Acme'>

          <!-- INSTALLDIR  指代实际的安装路径 -->
          <Directory Id='INSTALLDIR' Name='Foobar 1.0'></Directory>

          <!-- NOTE - 原来的配置删除掉 -->

        </Directory>
      </Directory>

        <!-- ..... -->
    </Directory>
	
    <!-- 控制上述文件定义如何安装，如 完整，典型，自定义 安装 -->
    <Feature Id='Complete' Title='Foobar 1.0' Description='The complete package.'
      Display='expand' Level='1' ConfigurableDirectory='INSTALLDIR'>

      <!-- 安装主程序 -->
      <Feature Id='MainProgram' Title='Program' Description='The main executable.' Level='1'>

        <!-- 引用由 heat.exe 生成的组 -->
        <ComponentGroupRef Id='HeatGroup' />
        <ComponentRef Id='ProgramMenuDir' />
      </Feature>
    </Feature>
	
    <!-- ... -->
  </Product>

  <!-- heat.exe 生成 -->
  <Fragment>
      <DirectoryRef Id="TARGETDIR">
          <Directory Id="dirCFD54C07EFB094CAD317543172D62CC3" Name="bin">
              <Component Id="cmp06A30E27D9903457396064AFC9CF112F" Guid="{C7380132-F932-4028-8E17-24CC563931A2}">
                  <File Id="filBEC2E0FE0BD7DFA9BA14C6438EBB5189" KeyPath="yes" Source="demo.exe" />
              </Component>
              <Component Id="cmpF6BD60E46FB06A48D7DADAAB1BA790EE" Guid="{DDB86648-6325-451B-BCEF-5FCA3E641459}">
                  <File Id="filDF56FC3F88D48EC87C1A314AC10051FC" KeyPath="yes" Source="readme.md" />
              </Component>
          </Directory>
      </DirectoryRef>
  </Fragment>
  <Fragment>
      <ComponentGroup Id="HeatGroup">
          <ComponentRef Id="cmp06A30E27D9903457396064AFC9CF112F" />
          <ComponentRef Id="cmpF6BD60E46FB06A48D7DADAAB1BA790EE" />
      </ComponentGroup>
  </Fragment>
</Wix>

```


# v4/v5

`wix v3` 是一些列工具的集合，到了 `wix v4/v5` 得到了优化，基本所有工具都被合并到了 `wix` 里

```term
triangle@LEARN:~$ wix -h
WiX Toolset version 5.0.2+aa65968c419420d32e3e1b647aea0082f5ca5b78
Copyright (c) .NET Foundation and contributors. All rights reserved.

Usage:
  wix [command] [options]

Options:
  -h|--help         Show command line help.
  --version         Display WiX Toolset version in use.
  --nologo          Suppress displaying the logo information.

Commands:
  build             Build a wixlib, package, or bundle.
  msi               Specialized operations for manipulating Windows Installer packages.
  burn              Specialized operations for manipulating Burn-based bundles.
  extension         Manage WiX extension cache.
  convert           Convert v3 source code to v4 source code.
  format            Ensure consistent formatting of source code.
triangle@LEARN:~$ wix build -o demo.msi demo.wxs
triangle@LEARN:~$ wix convert file.wxs  // 转换 v3 语法格式为 v4 或 v5
```

# HeatWave

[HeatWave](https://www.firegiant.com/docs/heatwave/) 对 `wix toolset` 工具的界面封装，不用自己手撸 `.wxs` 配置，可以在 `visual studio` 中配置使用。


