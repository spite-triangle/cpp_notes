---
title: "ESLint"
date: "2026-06-28"
draft: false
description: "ESLint 代码规范检测工具，支持 TypeScript 解析与自动化规则配置"
weight: 3
tags: ["eslint", "TypeScript-eslint", "代码规范", "规则配置", "linter"]
categories: ["前端工程化"]
---


# 简介

`ESLint` 是一个用于识别和报告 `JavaScript/TypeSrcipt` 代码中问题的工具。它可以帮助开发者发现潜在的错误、风格问题以及最佳实践。`ESLint` 可以与各种编辑器和构建工具集成，提供实时反馈和自动化修复功能。

```term
triangle@learn$ npm install eslint --save-dev
```

# 配置

`ESLint` 的配置文件通常命名为 `eslint.config.js`

```javascript
// eslint.config.js
import tseslint from 'typescript-eslint';

export default tseslint.config({
    // 1. 继承官方推荐和 TypeScript 推荐规则
    extends: [
        "eslint:recommended",
        ...tseslint.configs.recommended,
    ],
    // 2. 配置语言环境
    languageOptions: {
        parser: tseslint.parser, // 使用 TypeScript 解析器
        parserOptions: {
            project: ["./tsconfig.json"], // 为需要类型信息的规则提供路径
        },
    },
    // 3. 自定义规则
    rules: {
        "no-console": "warn",
        "@typescript-eslint/no-unused-vars": "error",
    },
    // 4. 忽略某些文件
    ignores: ["dist/**", "node_modules/**"],
});
```
