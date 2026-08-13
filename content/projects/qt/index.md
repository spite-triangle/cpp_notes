---
title: "Qt Servitor"
date: 2023-09-24
draft: false
description: "It's a friendly tool that help you to build Qt application comfortably with vscode."
summary: "在 vscode 中辅助 Qt 项目构建的友好插件。"
featured: true
tags:
  - vscode
  - Qt
  - QML
categories:
  - projects
cover: qt.png
status: "completion"
link: "https://marketplace.visualstudio.com/items?itemName=TriangleOxO.qtservitor"
---


# Qt Servitor

Qt Servitor 插件的开发目的是在 vscode 中辅助 Qt 项目的构建。

## 功能特性


- 自动配置 launch.json 文件，便于 Qt 程序的运行与调试；
- 自动配置 c_cpp_properties.json，使得 c/c++ extension 插件能正确识别 Qt 相关头文件
- Qt sdk 版本切换；
- Qt 模块自动生成；
- assistant.exe、designer.exe、linguist.exe 、windeployqt.exe、qml.exe 、qtcreator.exe 等工具的快捷启动。

## QML LSP

- QML 实时预览
- QML 语言特性
- 语法静态检测
- 拾色器
- 智能补全
- 定义跳转
- 变量重命名
- 引用查找
- 格式化