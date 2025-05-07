# 项目管理

# conda

## anaconda


1. 安装 [anaconda](https://www.anaconda.com/)
2. 利用`anaconda`低版本`Python`，例如 `Python 3.6.13 `

    ```term
    triangle@LEARN:~$ conda create --name 新环境名 python=版本号 
    triangle@LEARN:~$ conda activate 环境名 // 切换python版本
    triangle@LEARN:~$ conda deactivate // 退出当前Python环境，回到默认环境
    triangle@LEARN:~$ conda info --envs // 查看虚拟环境
    triangle@LEARN:~$ conda remove -n 环境名 --all // 删除虚拟环境
    ```
3. 将安装的Python添加到`jupyter`中 (base 不需要安装，自带)，<span style="color:red;font-weight:bold"> 一定要切换回基础环境，再向 jupyter 中添加内核 </span>
   ```term
    (base) triangle@LEARN:~$ conda activate python36 // 切换到新环境
    (python36) triangle@LEARN:~$ pip install ipykernel // 安装该版本的内核
    (python36) triangle@LEARN:~$ conda deactivate //  注意：一定要切换换回基础环境 
    (base) triangle@LEARN:~$ python -m ipykernel install --name python36 // 将新环境的内核添加到 jupyter 中
    (base) triangle@LEARN:~$ jupyter kernelspec list // 查看添加的内核
    Available kernels:
    python3     D:\Program\anaconda3\share\jupyter\kernels\python3
    python36    C:\ProgramData\jupyter\kernels\python36
    (base) triangle@LEARN:~$ jupyter kernelspec remove 内核名 // 移除内核
   ```
4. 在指定`Python`版本的环境中安装 `OpenCV`。<span style="color:red;font-weight:bold"> `opencv-contrib-python`直接用最新版，SIFT 的专利已经过期，用之前的`3.4.1`版本，可能有问题。`opencv-python`也可以直接用最新版</span>
    ```term
    triangle@LEARN:~$ pip install opencv-python opencv-contrib-python // 直接装最新的
    triangle@LEARN:~$ pip install opencv-python==版本号 opencv-contrib-python==版本号 // 选择版本
    ```

5. 替换源头

```term
triangle@LEARN:~$ conda info // 查看安装了哪些源
triangle@LEARN:~$ conda config --remove-key channels // 删除并恢复默认的conda源
triangle@LEARN:~$ conda config --add channels https-url // 添加源
triangle@LEARN:~$ conda config --remove channels https-url // 删除源
triangle@LEARN:~$ conda config --set show_channel_urls yes // 设置安装包时，显示镜像来源
```

除了命令行可以修改源，也可以直接改配置文件

> - window 路径: `C:\Users\[主机名]\.condarc`
> - linux 路径 : `~/.condarc`

```txt
channels:
  - defaults
  - conda-forge
show_channel_urls: true
default_channels:
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/r
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/msys2
custom_channels:
  conda-forge: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  msys2: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  bioconda: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  menpo: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  pytorch: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  pytorch-lts: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  simpleitk: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  deepmodeling: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/
```


## miniforge

1. 安装 [miniforge](https://github.com/conda-forge/miniforge)

2. 在系统的 `powershell` 或 `cmd` 直接使用 

```term
triangle@LEARN:~$ cd D:\ProgramData\miniforge3\condabin // 安装目录下的 condabin 文件夹
triangle@LEARN:~$ ./conda.bat init // 初始化环境
triangle@LEARN:~$ 
triangle@LEARN:~$ 
browserify : 无法加载文件 C:\Users\86136\AppData\Roaming\npm\browserify.ps1，因为在此系统上禁止运行脚本。有关详细信息，
请参阅 https:/go.microsoft.com/fwlink/?LinkID=135170 中的 about_Execution_Policies。
所在位置 行:1 字符: 1
    ...
(管理员) triangle@LEARN:~$ set-ExecutionPolicy RemoteSigned // 遇到上述异常，在「管理员」模式运行
```

3. 修改源，同 `anaconda` 配置

> [!note]
> miniforge 可以直接修改 `D:\ProgramData\miniforge3\.condarc`，不用再配置 `C:\Users\[主机名]\.condarc`


```txt
show_channel_urls: true
default_channels:
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/r
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/msys2
custom_channels:
  conda-forge: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  msys2: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  bioconda: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  menpo: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  pytorch: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  pytorch-lts: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  simpleitk: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  deepmodeling: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/
```


## mamba

>[!tip]
> `miniforge` 自带 `mamba` ，无需安装


`mamba` 是 `conda` 的下载加速版，用法基本同 `conda`。


```term
triangle@LEARN:~$ conda install -n base conda-libmamba-solver
triangle@LEARN:~$ conda config --set solver libmamba
triangle@LEARN:~$ conda install mamba -n base -c conda-forge // 安装 mamba
```

# 虚拟环境

## 概念

```txt
conda
├── python 3.8
│   ├── venv 1
│   └── venv 2
└── python 3.10
    ├── venv 1
    └── venv 2
```


`conda` 用于管理多个不同版本的 `pyhton` 环境，而「虚拟环境」则是在同一 `python` 版本下，「项目工程本地」搭建不同的 python 环境。

## venv

```term
triangle@LEARN:~$ python.exe -m venv .env // 创建虚拟环境，第三方包放在 .env 文件夹下
triangle@LEARN:~$ .\.env\Scripts\activate // windows 激活环境
triangle@LEARN:~$ source ./.env/bin/activate // linux 激活环境
(.env) triangle@LEARN:~$ pip list // 查看安装了哪些包
(.env) triangle@LEARN:~$ pip install fastapi
(.env) triangle@LEARN:~$ pip install "uvicorn[standard]"
(.env) triangle@LEARN:~$ deactivate // 退出激活环境
triangle@LEARN:~$ rm -rf .env // 直接删除文件夹即可删除虚拟环境
```


# pyproject.toml

## 介绍

在传统的 Python 项目中，我们往往需要维护多个配置文件：
- `setup.py` 用于项目打包、配置项目元数据
- `requirements.txt` 管理依赖
- 各种工具的配置文件（.pylintrc、pytest.ini 等）

而且不同的打包工具 `setup.py` 配置也可能存在差异。因此，`PEP 518` 定义了 `pyproject.toml` 文件的基本结构和构建系统规范,解决 Python 项目构建时的依赖问题，让构建过程变得更加可靠。


## 配置

```toml
# 项目元信息
[project]
name = "demo"                       # 需要是当前工程文件夹中的包名或脚本名
version = "0.1.0"
description = ""
authors = [{ name = "triangle" }]
license = { text = "MIT" }
readme = "README.md"
requires-python = ">=3.10"          # python 版本要求
dependencies = [                    # 正式环境依赖
    "requests>=2.25.0",
    "numpy>=1.20.0",
]           
optional-dependencies.test = [      # 测试框架附加依赖
    "pytest>=6.0.0",
    "pytest-cov>=2.0.0",
]
optional-dependencies.dev = [       # 开发环境附加依赖
    "black>=21.0",
    "flake8>=3.9.0",
]

# 项目 url
[project.urls]
Homepage = "https://example.com"
Repository = "https://github.com/example/your-package"

# 项目相关的快捷命令，项目 instal 成功后才可执行
[project.scripts]
hello = 'cli.hello:hello'                # 运行 cli/hello.py 中的 hello 函数

# 设置包管理构建系统，例如 poetry、setuptools 等
[build-system]
requires = ["poetry-core>=2.0.0,<3.0.0"]
build-backend = "poetry.core.masonry.api"

# 拓展的工具配置，例如配置 poetry、setuptools、black、isort、pytest 等
[tool.xxx]
xxx = 'xx'
```

```term
triangle@LEARN:~$ pip install -e . // 以 dev 模式安装项目 
triangle@LEARN:~$ hello // 执行 project.scripts 命令
```

## poetry

### 介绍

但是 `python` 官方只干了一半的活，**只定了协议，未提供工具**，因此，诞生了 [poetry](https://python-poetry.org/docs/)。使用 `poetry` 可以非常方便的管理「虚拟环境」与 `pyproject.toml`。

```term
triangle@LEARN:~$ pip install poetry 
triangle@LEARN:~$ poetry list
Commands
    new                     生成 package 工程的脚手架
    init                    初始化一个新的 pyproject.toml 文件
    install                 安装项目的依赖包
    env                     虚拟环境
    run                     在虚拟环境中运行命令
    shell                   进入项目的虚拟环境，2.1 版本后需要安装 pip install poetry-plugin-shell
    add                     添加依赖包到项目中
    remove                  移除项目中的依赖包
    update                  更新所有依赖
    show                    显示项目的依赖包信息
    source                  源管理
    export                  导出项目依赖到 requirements.txt 文件
    build                   打包，只能生成 .whl 与压缩包
    publish                 发布包到公网 
triangle@LEARN:~$ poetry export -f requirements.txt -o requirements.txt // 根据 pyproject.toml 生成 requirements.txt
```

### 虚拟环境

`poetry` 除了能进行包管理外，还可以配置虚拟环境

```term
triangle@LEARN:~$ poetry config --list
  ...
virtualenvs.create = true                       # 自动创建虚拟环境
virtualenvs.in-project = null                   # 虚拟环境在项目工程的 .venv 文件夹下
virtualenvs.path = "{cache-dir}\\virtualenvs"   # 默认虚拟环境路径
  ...

triangle@LEARN:~$ poetry config virtualenvs.in-project true // 虚拟环境创建在项目目录，方便管理
triangle@LEARN:~$ poetry env use python // 根据当前 python 版本创建虚拟环境
triangle@LEARN:~$ poetry env activate // 只是打印进入虚拟环境的命令，具体指令见 venv 章节
triangle@LEARN:~$ poetry run [command] // 在虚拟环境中执行 command 命令
```


### 配置文件

```term
triangle@LEARN:~$ poetry init // 生成 pyproject.toml
triangle@LEARN:~$ poetry.exe source add aliyun  https://mirrors.aliyun.com/pypi/simple // 添加源
triangle@LEARN:~$ poetry.exe source remove aliyun // 删除源
```


```toml
[tool.poetry]
package-mode = false  # 当前项目非 .whl 包开发，只使用 poetry 的包管理能力

# 配置 poetry 的代理源
[[tool.poetry.source]]
name = "aliyun"
url = "https://mirrors.aliyun.com/pypi/simple"
priority = "primary"
```

其他配置见 [Doc](https://python-poetry.org/docs/pyproject/)

### 依赖管理

`poetry` 区分 `dev` 与 `test` 依赖，是以 `group` 的形式实现，即 `dev` 与 `test` 是两个依赖组，而默认组则是「运行环境依赖」。此外，根据需要也可以自定义 `group`。

```term
triangle@LEARN:~$ poetry add [package] // 同 pip install [package] ，运行环境依赖
triangle@LEARN:~$ poetry add [package] --group dev // 开发环境依赖
triangle@LEARN:~$ poetry add [package] --group test // 测试环境依赖
triangle@LEARN:~$ poetry remove [package] // 删除包
triangle@LEARN:~$ poetry install --help
Options
  --with [group1,group2]        安装 group1,group2 以及默认依赖
  --without [group1,group2]     不安装 group1,group2
  --only [group]                只安装 group
  --only-root                   只安装项目，不安装依赖
  --no-root                     只安装依赖，不安装项目
```



```toml
# poetry.exe add aiohttp
[project]
dependencies = [
    "aiohttp (>=3.11.18,<4.0.0)"
]

# poetry.exe add numpy --group test
[tool.poetry.group.test.dependencies]
numpy = "^2.2.5"

# poetry.exe add pandas --group dev
[tool.poetry.group.dev.dependencies]
pandas = "^2.2.3"
```