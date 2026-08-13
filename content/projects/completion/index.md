---
title: "Copilot Completions"
date: 2026-05-22
draft: false
description: "Code completion powered by LLM — GHOST (FIM Inline Completion) and NES (Next Edit Suggestion)"
summary: "基于 LLM 的 VS Code 代码补全插件 — 同时支持 GHOST 内联补全和 NES 预测性编辑。"
featured: true
tags:
  - vscode
  - AI
categories:
  - projects
cover: copilot.png
status: "completion"
link: "https://marketplace.visualstudio.com/items?itemName=young-triangle.copilot-completions"
---


# Copilot Completion

基于 LLM 的 VS Code 代码补全插件 — 同时支持 **GHOST** FIM 内联补全和 **NES** 预测性编辑。

## 功能特性

### GHOST — FIM (Fill in the Middle) Inline Completion

- 在编辑器中以幽灵文本形式呈现内联补全建议
- 通过可配置的 FIM 提示模板将前缀/后缀上下文发送给模型
- 基于 Tree-sitter 的代码块解析，实现智能补全边界
- 可配置相似度阈值的后缀重叠裁剪
- 缓存与防抖机制，确保流畅的用户体验

### NES — Next Edit Suggestion

- 预测开发者在当前文件中**下一步的编辑位置和内容**（不限于光标位置）
- 围绕光标进行**编辑窗口**解析，支持合并冲突标记感知
- **光标跳转预测**：预测开发者下一步导航位置，**目前可用，但是预测会引入两次额外的请求。**
- **编辑意图分类**：高/中/低积极性过滤
- 响应后处理管道：边界标记解析 → 光标标签清除 → 行级差异 → 后缀重叠裁剪
- 多种响应格式处理器：编辑窗口、代码块、编辑意图、统一 XML、自定义差异补丁
