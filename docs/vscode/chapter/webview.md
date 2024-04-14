# Webview

# 简介

通过 `webview` 可以在 vscode 中引入 Web 技术，创建更复杂的交互界面。若把 vscode 比作是浏览器的话，webview 就等效于浏览器中的 `iframe` 控件。

> [!note]
> 由于 vscode 客户端本质上是基于 `NodeJs` 运行的，因此 vscode 并不完全等效于一个浏览器，同理 webview 也并不能完成支持网页界面的所有功能。**想要在 vscode 中运行一个真正的网页，则需要在 webview 中使用 `iframe` 加载网页。**

> - [webview-sample](https://github.com/microsoft/vscode-extension-samples/blob/main/webview-sample/README.md)
> - [webview-view-sample](https://github.com/microsoft/vscode-extension-samples/tree/main/webview-view-sample)

# API

## 界面创建

```ts
import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
    context.subscriptions.push(
        vscode.commands.registerCommand('catCoding.start', () => {
            // 配置一个 panel
            const panel = vscode.window.createWebviewPanel(
                'catCoding',
                'Cat Coding',
                vscode.ViewColumn.One,
                {
                    // webview 是否允许加载 js
                    enableScripts: true,
                    // webview 能访问的资源
                    // 默认是 插件文件夹 + 当前工作区间
                    localResourceRoots: []
                }
            );

            // 添加需要展示的 html 内容
            panel.webview.html = getWebviewContent(panel);

            // 是否可见
            panel.visible;

            // 显示 webview 到指定窗口
            // vscode.ViewColumn.One
            const columnToShowIn = vscode.window.activeTextEditor ? vscode.window.activeTextEditor.viewColumn : undefined;
            panel.reveal(columnToShowIn);

            // visible 改变时，会触发该事件
            panel.onDidChangeViewState(e=>{

            }, null, context.subscriptions);
        })
    );
}

function getWebviewContent(panel: vscode.WebviewPanel) {
    // 通过 asWebviewUri 将 vscode.uri 转换为 webview 可以识别的 uri
    const scriptUri = panel.webview.asWebviewUri(vscode.Uri.joinPath(this._extensionUri, 'media', 'main.js'));

    return `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cat Coding</title>
</head>
<body>
    <img src="https://media.giphy.com/media/JIX9t2j0ZTN9S/giphy.gif" width="300" />

    // NOTE - 加载转换后的路径
    <script src="${scriptUri}"></srcipt>
</body>
</html>`;
}
```

## 消息通信 

- vscode 发送，webview 接收

```ts
// vscode 发送消息
panel.webview.postMessage({text: 'hello webview'});

// webview 接收
window.addEventListener('message', event => {
    const message = event.data;
	console.log('Webview接收到的消息：', message);
}
```

- webview 发送，vscode 接收

```ts
// webview 发送
const vscode = acquireVsCodeApi();
vscode.postMessage({text: 'hello vscode'});

// vscode 接收
panel.webview.onDidReceiveMessage(message => {
	console.log('插件收到的消息：', message);
}, undefined, context.subscriptions);
```

# 生命周期

webview 属于创建它的 Extension，因此 Extension 必须对所有通过 `vscode.window.createWebviewPanel` 创建的 webview 负责。当用户主动关闭 webview 后，webview 将被销毁，而插件也不能再使用该 webview 。若 Extension 访问已经销毁的 webview ，vscode 将会抛出异常。

```ts
import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
  context.subscriptions.push(
    vscode.commands.registerCommand('catCoding.start', () => {
      const panel = vscode.window.createWebviewPanel(
        'catCoding',
        'Cat Coding',
        vscode.ViewColumn.One,
        {}
      );
      
      // 定时器引用了 webview
      // 通过 dispose() 可以主动关闭 webview
      const timeout = setTimeout(() => panel.dispose(), 5000);

      // 当 webview 销毁时，可以通过 onDidDispose 来释放资源，解除 Extension 对该 webview 的引用
      panel.onDidDispose(
        () => {
            clearTimeout(timeout);
        },
        null,
        context.subscriptions
      );
      
    })
  );
}
```

webview 的生命周期以 `createWebviewPanel` 开始，当用户主动关闭或调用 `.dispose()`结束，**但是 webview 中内容的生命周期是以 webview 展示在界面上开始，webview 不可见结束**。因此，vscode 提供了 `getState` 与 `setState` 来储存 webview 中的内容，这些数据会在 webview 销毁时，被释放。

```ts
// Inside a webview script
const vscode = acquireVsCodeApi();

// 储存数据
vscode.setState({ count });

//  读取
const previousState = vscode.getState();
let count = previousState ? previousState.count : 0;
```

也可以通过配置让 webview 中的内容永远存活，但是这样会占用更多的资源

```ts
    const panel = vscode.window.createWebviewPanel(
        'catCoding',
        'Cat Coding',
        vscode.ViewColumn.One,
        {
            enableScripts: true,
            retainContextWhenHidden: true // 当 webview 被隐藏时，webview 中的内容依然运行
        }
    );
```


# 菜单

```json
"contributes": {
  "menus": {
    // 菜单
    "webview/context": [
      {
        "command": "catCoding.yarn",
        "when": "webviewId == 'catCoding'"
      },
      {
        "command": "catCoding.insertLion",
        "when": "webviewId == 'catCoding' && webviewSection == 'editor'"
      }
    ]
  },
  // 菜单指令
  "commands": [
    {
      "command": "catCoding.yarn",
      "title": "Yarn 🧶",
      "category": "Cat Coding"
    },
    {
      "command": "catCoding.insertLion",
      "title": "Insert 🦁",
      "category": "Cat Coding"
    },
    ...
  ]
}
```

在 webview ，可以利用 `data-vscode-context` 为 HTML 空间添加额外的属性，例如指定 `webviewSection`

```html
<!-- webviewSection : main  -->
<div class="main" data-vscode-context='{"webviewSection": "main", "mouseCount": 4}'>
    <h1>Cat Coding</h1>

    <!-- preventDefaultContextMenuItems : 不展示默认的 copy 和 paste 菜单 -->
    <textarea data-vscode-context='{"webviewSection": "editor", "preventDefaultContextMenuItems": true}'></textarea>
</div>
```


