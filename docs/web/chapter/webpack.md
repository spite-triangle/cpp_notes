
# webpack

# 简介

[webpack](https://blog.csdn.net/chen4565/article/details/118093586) 是一个构建工具，为了将开发好的代码进行优化压缩，得到体积更小、语法统一更利于运行的代码。
- **代码压缩**: 将 js、css 混淆压缩
- **编译语法**: 编写JS时使用ES6、TypeScript等语法，需要转换为浏览器能识别的规则。
- **处理模块化**: 浏览器不支持 `import`，需要转换为 `require()`

> [!note]
> 针对 `React` 与 `vue` 项目，现在更推荐使用 `vite`

# 安装

```term
triangle@LEARN:~$ npm install webpack webpack-cli -g
triangle@LEARN:~$ npm install css-loader style-loader // 安装 loader ，可选
```

# 配置

配置文件 `webpack.config.js`

```javascript
const path = require('path') // 引用path模块
module.exports = {
    entry: "./main.js", // 程序入口 js
    output: {
        path: path.resolve(__dirname,'build'),  // 输出路径
        filename: "out.js", // 打包输出结果

        // NOTE - 需要从 out.js 中 export 内容时，才使用
        library: "libraryName", // 导出的库名
        libraryTarget:"type", // 库的提供方式：var、commonjs、commonjs2、this、window
    },
    module: {
        rules: [ 
            { 
                // 匹配文件
                test: /\.css$/, 
                // loader执行顺序, 从右到左, 从下到上, 依次执行
                use:[
                    'style-loader',
                    'css-loader'
                ]
            }
        ]
    },
    // 打包模式
    mode:"development"
};
```

指定 `library` 与 `libraryTarget` 后，能导出 `export` 修饰的内容给外部使用，即[库](https://blog.csdn.net/qq_17175013/article/details/119753486)。若不指定，导出的`js` 文件则是单纯的可执行脚本，且内部元素不会向外暴露。

```javascript
/* 不指定库时的打包结果，外部无法调用 */
(
    var xx;
    dosomething();
)();
```

# 打包

```term
triangle@LEARN:~$ webpack // 配置了 webpack.config.js 直接运行
triangle@LEARN:~$ webpack ./index.js -o ./bin/out.js // 命令行
Options:
    -o                  打包输出
    --mode              指定打包模式 development, production
```
