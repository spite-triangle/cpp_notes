# 基本概念

- [官方API](https://code.visualstudio.com/api)
- [官方案例](https://github.com/microsoft/vscode-extension-samples)
- [中文翻译](https://rackar.github.io/vscode-ext-doccn/)

# 项目管理

## 项目创建

```term
triangle@LEARN:~$ npm install -g yo generator-code // 项目脚手架
triangle@LEARN:~$ yo code // 配置项目
triangle@LEARN:~$ tree -L 1
.
├── CHANGELOG.md
├── README.md
├── package Config.json
├── package-lock.json
├── package.json                    # 配置项目文件 
├── src								# 插件代码
├── tsconfig.json                   # ts 配置文件
└── vsc-extension-quickstart.md
```

## 项目配置

> [!note]
> [界面 UI](https://code.visualstudio.com/api/ux-guidelines/overview) 交互都是通过 [contributes](https://code.visualstudio.com/api/references/contribution-points) 进行配置

```json
/* ================================== package.json ================================= */
{
	// name.publisher 作为插件的唯一ID
	"name": "testui",
	"publisher": "triangle",
	"displayName": "testUI",
	"description": "",
	"version": "0.0.1",
	// NOTE - vscode 版本号，会检测vscode版本；postinstall 根据对应版本检测 vscode 语法
	"engines": {
		"vscode": "^1.69.0"
	},
	"categories": [
		"Other"
	],
	// 啥时候启动插件
	"activationEvents": [
		"onCommand:testui.helloWorld"
	],
	// 插件入口
	"main": "./out/extension.js",

	// 插件配置
	"contributes": {
		// 可以在在 ctrl shit p 命令面板找到 title 选项，title 选项可以启动 command 
		"commands": [
			{
				"command": "testui.helloWorld",
				"title": "Hello World"
			}
		],

		"menus": {
			// 限制命令在 ctrl shit p 中的出现条件
			"commandPalette": [
				{
					"command": "testui.helloWorld",
					"when": "editorLangId == cpp"
				}
			]
		}
	},

	// npm run 脚本
	"scripts": {
		"vscode:prepublish": "npm run compile",
		"compile": "tsc -p ./",
		"watch": "tsc -watch -p ./",
		"postinstall": "node ./node_modules/vscode/bin/install",
		"pretest": "npm run compile && npm run lint",
		"lint": "eslint src --ext ts",
		"test": "node ./out/test/runTest.js"
	},

	// 开发依赖包
	"devDependencies": {
		"@types/vscode": "^1.76.0",
		"@types/glob": "^7.2.0",
		"@types/mocha": "^9.1.1",
		"@types/node": "14.x",
		"@typescript-eslint/eslint-plugin": "^5.21.0",
		"@typescript-eslint/parser": "^5.21.0",
		"eslint": "^8.14.0",
		"glob": "^8.0.1",
		"mocha": "^9.2.2",
		"typescript": "^4.6.4",
		"@vscode/test-electron": "^2.1.3"
	}
}
```


## 打包发布

>[!tip]
> [打包发布到 vscode 插件商店](https://code.visualstudio.com/api/working-with-extensions/publishing-extension)

```term
triangle@LEARN:~$ npm install -g @vscode/vsce // 安装打包工具
triangle@LEARN:~$ vsce package // 打包 .vsix 文件
triangle@LEARN:~$ vsce publish // 发布
```


# Hello World

>[!tip]
> [第一个插件](https://rackar.github.io/vscode-ext-doccn/get-started/your-first-extension.html#%E5%BC%80%E5%8F%91%E6%8F%92%E4%BB%B6)


```ts
// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
import * as vscode from 'vscode';

// this method is called when your extension is activated
// your extension is activated the very first time the command is executed
export function activate(context: vscode.ExtensionContext) {

	// 注册命令，testui.helloWorld 对应 package.json 下的 commands
	disposable = vscode.commands.registerCommand('testui.helloWorld', () => {
		vscode.window.showInformationMessage("hello world");
	});
	context.subscriptions.push(disposable);
}

// this method is called when your extension is deactivated
export function deactivate() {}
```

