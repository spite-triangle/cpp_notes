# 编译器

# babel

[babel](https://babeljs.io/) 是一个 JavaScript 编译器，主要用于将 `ECMAScript 2015+` 代码转换为向后兼容的 `JavaScript` 版本，**对 `TypeScript` 支持较差**

```term
triangle@ubuntu:~/$ npm install --save-dev @babel/core @babel/cli @babel/preset-env
triangle@ubuntu:~/$ npm install --save-dev babel-plugin-polyfill-corejs3 core-js@3
```

功能
- `parse`: 将代码解析为 `AST`
- `traverse`: 遍历 `AST`
- `generate`: 将 `AST` 生成为代码
- `transform`: 将输入代码转换为 `target` 代码，**用于兼容低版本代码**

使用

```js
import * as Babel from '@babel/core';
import presetEnv from '@babel/preset-env';

// 将输入代码转换为 target 输出代码
const result = Babel.transformFileSync(
    './test.js', 
    {
        presets: [ 
            [presetEnv ,{
                targets: {
                    ie: '11',        // 转换代码要支持 IE 11
                    chrome: '50',
                }
            }]
        ],
        plugins: [
            ['babel-plugin-polyfill-corejs3', { 
                method: 'usage-global',         // 类似 useBuiltIns: 'usage'
                version: '3'                    // 对应 core-js@3
            }]
        ]
    }
);

console.log(result.code);
```

# SWC

[swc](https://swc.rs/) 是一个 `Rust` 编写的 `JavaScript` 和 `TypeScript` 编译器，旨在提供高性能的编译速度
- 速度比 `babel` 快 20 倍
- 支持 `TypeScript`
- `API` 更精简

```term
triangle@ubuntu:~/$ npm i -D @swc/cli @swc/core
```

使用

```js
import swc from '@swc/core'

const result = swc.transformFileSync('./test.js', {
   jsc: {
       target: "es5", //代码转换es5
       parser: {
           syntax: 'ecmascript'
       }
   }
})
console.log(result.code)
```