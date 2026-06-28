
# TypeScript

# tsc

```term
triangle@LEARN:~$ npm i -g typescript // 安装
triangle@LEARN:~$ tsc --init // 初始化生成 tsconfig.json 文件，需要配合 npm init 一起初始化项目
triangle@LEARN:~$ tsc --help
Options
    -p              根据 tsconfig.json 编译项目
    -w              监视文件
```

- **tsconfig.json**

```json
{
    "compilerOptions": {
        "module": "commonjs",   // 生成的 javascript 格式
        "target": "ES2020",
        "outDir": "out",        // js 输出目录
        "lib": [                // 语法支持库
            "ES2020",
            "ES2021.String"
        ],
        "sourceMap": true,      // js 输出文件可以映射到 ts
        "rootDir": "src",       // ts 文件所在文件夹
        "strict": true   /* enable all strict type-checking options */
        /* Additional Checks */
        // "noImplicitReturns": true, /* Report error when not all code paths in function return a value. */
        // "noFallthroughCasesInSwitch": true, /* Report errors for fallthrough cases in switch statement. */
        // "noUnusedParameters": true,  /* Report errors on unused parameters. */
    }
}
```

- **package.json**

```json
{
  "name": "vscode-util",
  "version": "1.0.0",
  "description": "",
  "main": "out/main.js",
  "scripts": {
    "compile": "tsc -p ./",             // tsc 编译
    "watch": "tsc -watch -p ./",        // tsc 实时监控编译
  },
  "author": "",
  "license": "ISC",
  "dependencies": {
    "util-kit": "^0.1.4"
  }
}

```

# 多模块

```term
triangle@LEARN:~$ tree .
    .
    ├── package.json        # 项目配置
    ├── tsconfig.base.json  # 根 typescript 配置
    ├── shared/             # 共享代码
    │   └── <code ts>
    ├── module1/
    │   ├── src/
    │   │   └── <code ts>
    │   └── tsconfig.json   # module1 的 ts 配置
    └──  module2/
        ├── src/
        │   └── <code ts>
        └── tsconfig.json   # module2 的 ts 配置
```

- **tsconfig.base.json** ： 公共配置

```json
{
	"compilerOptions": {
		"module": "Node16",
		"target": "ES2022",
		"lib": [
			"ES2022"
		],
		"sourceMap": true,
		"strict": true,   /* enable all strict type-checking options */

        /* 
            公共代码路径配置
            import { Foo } from '~shared/foo';
        */
        "paths": {
            "~shared/*": ["../shared/*"]
        },
	}
}
```

- **tsconfig.json**

```json
{
    // 配置当前模块的编译选项
	"compilerOptions": {
		"outDir": "../../out/extension",
		"rootDir": "./",
	},

    // 继承基础配置
	"extends": "../tsconfig.base.json"
}

```