---
title: "windbg"
description: "Windows 系统级调试与诊断工具，涵盖栈内存分析、线程调度跟踪及 Application Verifier 性能测试。"
tags: ["Windbg", "内核调试", "Windows SDK", "性能分析"]
categories: ["开发工具"]
series: ["framework"]
cover: images/windbg.png
---

<!-- VOLUME_OVERVIEW_START -->
本卷系统梳理 Windows 平台下的调试与诊断技术体系，从底层原理到工具实战层层递进。理论基础章节讲解软硬件断点机制、异常分发流程及 SEH 模型；Windbg 命令篇覆盖核心语法、别名管理与远程调试配置。性能分析篇介绍 PerfView、WPA 等 ETW 工具链的使用，可调试性设计则涵盖日志记录与转储文件生成技术。此外，本卷深入线程数据结构（EPROCESS/TEB）、栈帧结构与 Cookie 保护机制、堆管理器及页堆泄漏检测，最后通过 Application Verifier 的运行时 API Hook 验证完成对内存安全问题的闭环诊断。
<!-- VOLUME_OVERVIEW_END -->
