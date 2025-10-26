# API


# 通用模式

## 基础模块

通过 [util-kit](https://www.npmjs.com/package/util-kit) 库可以对 vscode 的核心基础模块实现进行测试。

```term
triangle@LEARN:~$ npm install --S util-kit
```


## Disposables

```ts
export interface IDisposable {
    // 析构函数
    dispose(): void;
}
```

vscode 通过 `IDisposable` 可以构建对象析构树，树的根部对象能够调用子对象的 `dispose()` 对其进行销毁（即回收资源）。析构会从调用 `dispose()` 的节点一直传递到根部叶子节点, **且一个对象只能被 `dispose` 一次**。


```ts
import { 
    DisposableStore, toDisposable, 
} from 'util-kit';


console.log("======== create ==========");
let a = toDisposable(()=>{
    console.log("a");
})
let b = toDisposable(()=>{
    console.log("b");
})
let c = toDisposable(()=>{
    console.log("c");
})

console.log("======== link ==========");

let lst = new DisposableStore();
let sublst = new DisposableStore();

sublst.add(a);

lst.add(a);
lst.add(b);
lst.add(c);
lst.add(sublst);

console.log("======== dispose ==========");

lst.dispose();
```

```term
triangle@LEARN:~$ tsc -p ./ && node ./out/main.js
======== create ==========
======== link ==========
======== dispose ==========
a
b
c
a
```

## Promises

```ts
// Thenable 对 Promise 进行了封装，T 为结果
interface Thenable<T> {
    /**
    * Attaches callbacks for the resolution and/or rejection of the Promise.
    * @param onfulfilled The callback to execute when the Promise is resolved.
    * @param onrejected The callback to execute when the Promise is rejected.
    * @returns A Promise for the completion of which ever callback is executed.
    */
    then<TResult>(onfulfilled?: (value: T) => TResult | Thenable<TResult>, onrejected?: (reason: any) => TResult | Thenable<TResult>): Thenable<TResult>;
    then<TResult>(onfulfilled?: (value: T) => TResult | Thenable<TResult>, onrejected?: (reason: any) => void): Thenable<TResult>;
}


function showSelectCombox() {
    // 选择输入框
    const items = ["fuck1","fuck2","fuck3"];
    const res:Thenable<string[] | undefined> = vscode.window.showQuickPick(items);

    // 展示选择的结果
    res.then((value)=>{
        if (value != undefined){
            vscode.window.showInformationMessage(value);
        }
    });
}
```

## Cancellation

**作用：** 可以用于取消 `Promise` 或长时间运行的操作，例如请求超时，取消用户正输入时的代码补全运算。

```ts
/* 取消信号实体 */
export interface CancellationToken {

    /**
     * Is `true` when the token has been cancelled, `false` otherwise. 且状态变化不可逆
     */
    isCancellationRequested: boolean;

    /**
     * An {@link Event} which fires upon cancellation.
     */
    onCancellationRequested: Event<any>;
}

/* 取消信号的控制体 */
export class CancellationTokenSource {

    /* 受控制的取消信号实体 */
    token: CancellationToken;

    /**
     * Signal cancellation on the token.
     */
    cancel(): void;

    /**
     * Dispose object and free resources.
     */
    dispose(): void;
}


function showSelectCombox() {
    // 选择输入框
    const items = ["fuck1","fuck2","fuck3"];

    // 创建 CancellationTokenSource
    let cancel = new CancellationTokenSource();

    vscode.window.showQuickPick(items,undefined, cancel.token)
    .then(
        (val)=>{
            console.log("ok");
        }, (e)=>{
            console.log("fck");
        }
    );

    // 取消 vscode.window.showQuickPick 操作
    cancel.cancel();

    // 释放资源
    cancel.dispose();
}
```

## CancelablePromise


vscode 内部将 `Promise` 与 `Cancellation` 机制进行结合，得到了 `CancelablePromise` ，实现利用 `Cancellation` 取消 `Promise`。

```ts
/* 利用 util-kit 库进行测试*/
import { asyncs } from 'util-kit';
const { createCancelablePromise} = asyncs;

const order: string[] = [];

// 创建一个 CancelablePromise 实例
let cancellablePromise = createCancelablePromise(token => {
    order.push('init');

    // 配置 Cancellation 
    token.onCancellationRequested(_ => order.push('cancelled'));

    // 创建 promise
    return new Promise((resolve, reject)=>{
        resolve(250);
    });
});

order.push('afterCreate');

let promise = cancellablePromise.finally(() => order.push('finally'));

// Cancellation 取消
cancellablePromise.cancel();
order.push('afterCancel');

promise.then(
    (val) => { 
        // [ 'init', 'afterCreate', 'afterCancel', 'finally' ]
        console.log(order);
    },
    (reason) => {
        //  [ 'init', 'afterCreate', 'cancelled', 'afterCancel', 'finally' ]
        console.log(order);
    }
);
```


## Emitter

```ts
import { Event, Emitter, IDisposable } from 'util-kit';

function handler(value: string) {
    console.log("handle event");
}

// 创建一个 emitter 对象
const emitter: Emitter<string> = new Emitter<string>();

// 注册处理函数
const subscription: IDisposable = emitter.event(handler);

// 触发事件
emitter.fire('event data');

// 释放 emitter 对象，不再执行 handler
subscription.dispose();
```

vscode 通过 [Emitter](vscode/chapter/event.md) 对象来管理事件，一个对象实例代表了一个 `Event` ，可实现事件的注册与触发。


```ts
// 接收器
var listener = function(event) {
  console.log('It happened', event);
};

// 创建文件系统监控
let fsWatcher = vscode.workspace.createFileSystemWatcher('**/*.js');

// 向 onDidDelete 信号注册一个接收器
var subscription = fsWatcher.onDidDelete(listener);

// 关闭接收器
subscription.dispose(); 
```

全局信号遵从以下命名规范

```txt
模式
    vscode.xxx.on[Will|Did]VerbNoun?
        xxx : 模块
        on : 表明是信号
        - will : 事件发生前
        - Did : 事件发生后
        - Did : 事件发生后
        - Ver : 什么事件
        - Noun : 谁产生的事件，
案例
    vscode.window.onDidChangeActiveTextEditor
```


# 配置

## configuration

在 vscode 中，自带配置文件 (例如 `settings.json` 、`launch.json`、`tasks.json`) 可以直接通过官方 API 进行修改

>[!note]
> 首先需要先在 `package.json` 中定义 `settings.json` 、`launch.json`、`tasks.json` 的配置，才能通过 `API` 进行访问，[官方教程](https://code.visualstudio.com/api/references/contribution-points#contributes.configuration)

```ts

// 获取配置
vscode.workspace.getConfiguration(section?: string, scope?: ConfigurationScope | null): WorkspaceConfiguration;

// 配置文件管理器
interface WorkspaceConfiguration{

    // 获取 section 配置
    get<T>(section: string, defaultValue: T): T;

    // 检索 section
    has(section: string): boolean;
    inspect<T>(section: string);

    // 更新配置
    update(section: string, value: any, configurationTarget?, overrideInLanguage?: boolean): Thenable<void>;
}
```


- **settings.json**

```ts
// 默认获取 settings.json 配置
let config = vscode.workspace.getConfiguration();

/* 
例如：
    {
        myExt: { 
            setting: { doIt: true }
        }
    }
想要获取 doIt 的结果，就可以设置 section identifier 为 myExt.setting
*/
let val = vscode.workspace.getConfiguration("myExt.setting").get("doIt");
```

- **launch.json** 与 **tasks.json**

```ts
// 使用 launch 与 tasks 作为节标识符号（section identifier）
// launch.json configuration
const config = workspace.getConfiguration('launch', vscode.workspace.workspaceFolders[0].uri);

// retrieve values
const values = config.get('configurations');
```

## json

除 `settings.json` 、`launch.json`、`tasks.json` 配置文件外，用户自定义的配置文件将不能使用 vscode 提供的接口，需要借助 `comment-json` 包进行解析。

```ts
import * as fs from 'fs';
import * as jsonc from 'comment-json';

// 读
const strConfig: string = fs.readFileSync(strPath, 'utf8');

// 解析
let json = jsonc.parse(strConfig, undefined, true);

// 写
let strContext = jsonc.stringify(json, null, 4);
fs.writeFile(strPath, strContext, (err)=>{
    if(err) Logger.ERROR(err.message);
});
```

## 数据存储

vscode 也为插件提供了数据存储能力。

```ts
export function activate(context: vscode.ExtensionContext){
    // Memento 储存的是 key-value 信息
    let mem1: vscode.Memento = context.workspaceState; // 局部
    let mem2: vscode.Memento = contex.globalState; // 全局

    let uri1: vscode.Url = context.globalStorageUri; 
    let uri2: vscode.Url = context.storageUri;
}
```

# 终端

## 终端创建

```ts
// 创建一个自定义终端
interface TerminalOptions {
    // "终端名";
    name?: string;
    // shell配置
    shellPath?: string;
    shellArgs?: string[] | string; 
    // 工作目录
    cwd?: string | Uri;
    // 环境变量
    // NOTE - process.env 获取系统环境变量，修改后再赋值给 env
    env?: { [key: string]: string | null | undefined };
    // 终端被创建在哪个位置
    location?: TerminalLocation | TerminalEditorLocationOptions | TerminalSplitLocationOptions;
}

// NOTE - 终端创建已经放到 vscode.window 中，返回的 Terminal 只是终端的句柄（引用），并非终端实例
vscode.window.createTerminal(options: TerminalOptions): Terminal;
```

##  终端使用

```ts
let terminal = vscode.window.createTerminal(options: TerminalOptions);

// 展示终端。只是展示，终端已经创建好了
// preserveFocus: false, 将鼠标移动到终端
terminal.show(preserveFocus?: boolean);

// 隐藏
terminal.hide();

// 发送一条文本到终端的 stdin，即向终端中输入一条指令
// addNewLine：发送的文本后面添加 `\n` 或者 `\r\n`
terminal.sendText(text: string, addNewLine?: boolean);

// 摧毁
terminal.dispose();
```

## 事件注册

```ts
// 当前活跃的终端被改变
vscode.window.onDidChangeActiveTerminali
// 有终端被关闭
vscode.window.onDidCloseTerminal
// 有新的终端被打开
vscode.window.onDidOpenTerminal
```
