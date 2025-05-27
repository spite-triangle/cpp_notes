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

2. 在 `windows` 系统中初始化

```term
triangle@LEARN:~$ cd miniforge3\condabin // 安装目录下的 condabin 文件夹
triangle@LEARN:~$ ./conda.bat init // 初始化环境
triangle@LEARN:~$ 
triangle@LEARN:~$ 
browserify : 无法加载文件 C:\Users\86136\AppData\Roaming\npm\browserify.ps1，因为在此系统上禁止运行脚本。有关详细信息，
请参阅 https:/go.microsoft.com/fwlink/?LinkID=135170 中的 about_Execution_Policies。
所在位置 行:1 字符: 1
    ...
(管理员) triangle@LEARN:~$ set-ExecutionPolicy RemoteSigned // 遇到上述异常，在「管理员」模式运行
```

3. 在 `linux` 系统中初始化

```term
triangle@LEARN:~$ cd miniforge3/bin // 安装目录的 bin 文件夹
triangle@LEARN:~$ ./conda init // 初始化
triangle@LEARN:~$ source ~/.brashrc
```

4. 修改源，同 `anaconda` 配置
 - `windows` :  `.\miniforge3\.condarc` 与 `C:\Users\[主机名]\.condarc`
 - `linux` : `~/.condarc` 与 `/etc/conda/.condarc`
 


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
    shell                   进入项目的虚拟环境
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
triangle@LEARN:~$ poetry env use python // 根据当前 python 版本创建虚拟环境，也可以指定 python.exe 的绝对路径
triangle@LEARN:~$ poetry env activate // 只是打印进入虚拟环境的命令，具体指令见 venv 章节
triangle@LEARN:~$ poetry run [command] // 在虚拟环境中执行 command 命令
```

若当前环境处于 `conda` 的虚拟环境下时，会直接使用 `conda` 的虚拟环境，不会再创建 `.venv` 虚拟环境

> [!note]
> 解决方案
> 1. 使用 `conda` 安装目标版本的 python （`poetry python install` 也能装，但是 `poetry` 不能完全替换源，会使用默认的 `pypi.org`）
> 2. `conda conda info --envs` 查看 python 的安装路径
> 3. `poetry env use [python]` 根据 conda 安装的 python 创建虚拟环境


### 工具配置

```term
triangle@LEARN:~$ poetry init // 生成 pyproject.toml
triangle@LEARN:~$ poetry.exe source add aliyun  xxxxx // 添加源
triangle@LEARN:~$ poetry.exe source remove aliyun // 删除源
triangle@LEARN:~$ poetry.exe config --list
    ...
installer.max-workers = null            # 依赖安装时的并发数
installer.parallel = true               # 并发安装依赖
installer.re-resolve = true
repositories.aliyun.url = "xxxxx"  # 配置的源
    ...
```


```toml
[tool.poetry]
package-mode = false  # 当前项目非 .whl 包开发，只使用 poetry 的包管理能力

# 配置 poetry 的代理源，使用 'poetry.exe source' 自动管理
[[tool.poetry.source]]
name = "aliyun"
url = "xxxx"
priority = "primary"
default = true          # 无法安装时才设置。使 poetry 的默认回调行为都使用上述源
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

### poetry.lock

`poetry.lock` 锁定文件存在以下作用：
- 记录确切的版本号，保证每个人安装的库版本都一样
- 库依赖关系的准确记录
- 可通过锁定文件检查 `pyproject.toml` 的正确性
- 可用于 `git` 、`svn` 版本控制，防止开发人员的本地依赖库版本不统一

在执行 `poetry add`，`poetry remove`，`poetry update` 命令后，会自动更新 `poetry.lock`。但有时会直接修改 `pyproject.toml`，这就导致 `pyproject.toml` 与 `poetry.lock` 不一致，这种情况下可以手动更新

```term
triangle@LEARN:~$ poetry update --lock
triangle@LEARN:~$ poetry lock
triangle@LEARN:~$ poetry check  // 检查 poetry.lock 与 pyproject.toml
```

### 插件

在新版的 poetry 中，一些指令是通过插件的形式提供，例如 `export`、`shell`等

```toml
# 该配置已经无效
[tool.poetry.requires-plugins]
poetry-plugin-export = ">=1.8"
poetry-plugin-shell = ">=1.0"
```

**但是插件无法通过 `poetry install` 自动安装**

```term
triangle@LEARN:~$ pip install poetry-plugin-* // 安装插件
```

若想实现 `npm run [command]` 的效果，需要借助脚本实现

```term
triangle@LEARN:~$ pip install poetry-plugin-taskipy // 安装方案一
triangle@LEARN:~$ poetry self add poetry-plugin-taskipy // 安装方案二，不能修改默认源
```

```toml
# pyproject.toml 中配置
[tool.taskipy.tasks]
test_echo = "echo 'test'"
```

```term
triangle@LEARN:~$ poetry task test_echo // 执行
test
```

## pdm

### 介绍

[pdm](https://github.com/pdm-project/pdm) 与 `poetry` 一样，通过 `pyproject.toml` 进行项目管理。pdm 的设计大部分功能都借鉴了 poetry ，且对 poetry 的不足之处进行了优化，使用更加简便，但目前 pdm 用户量没有 poetry 多，属于战未来的产品。

```term
triangle@LEARN:~$ pip install pdm
triangle@LEARN:~$ pdm --help
Commands
  init                  Initialize a pyproject.toml for PDM. Built-in templates: - default: `pdm init`, A simple template with a basic structure. - minimal: `pdm init minimal`, A minimal template with only `pyproject.toml`.
  new                   Create a new Python project at <project_path>
  install               Install dependencies from lock file
  add                   Add package(s) to pyproject.toml and install them
  remove                Remove packages from pyproject.toml
  search                Search for PyPI packages
  list                  List packages installed in the current working set
  show                  Show the package information
  config                Display the current configuration
  build                 Build artifacts for distribution
  cache                 Control the caches of PDM
  completion            Generate completion scripts for the given shell
  export                Export the locked packages set to other formats
  fix                   Fix the project problems according to the latest version of PDM
  import                Import project metadata from other formats
  info                  Show the project information
  lock                  Resolve and lock dependencies
  outdated              Check for outdated packages and list the latest versions on indexes.
  publish               Build and publish the project to PyPI
  python (py)           Manage installed Python interpreters
  run                   Run commands or scripts with local packages loaded
  self (plugin)         Manage the PDM program itself (previously known as plugin)
  sync                  Synchronize the current working set with lock file
  update                Update package(s) in pyproject.toml
  use                   Use the given python version or path as base interpreter. If not found, PDM will try to install one.
  venv                  Virtualenv management
```

### 虚拟环境

pdm 可以启用 `PEP 582` 来创建隔离的开发环境，**但是 PEP 582 规则被拒绝了**。因此，还是推荐使用虚拟环境的方式来创建隔离开发环境，pdm 支持以下方式
- `virtualenv` : 默认方式，是最早诞生的虚拟环境工具，兼容性好
- `venv` : 在 `python 3.3` 后集成的默认虚拟环境创建工具，**推荐**
- `conda` 
- `uv`

```term
triangle@LEARN:~$ pdm config 
Configure
    venv.backend [virtualenv|venv|conda]            修改虚拟环境创建方式
    venv.with_pip                                   虚拟环境是否安装 pip
    venv.in_project                                 虚拟环境创建在工程文件夹
triangle@LEARN:~$ pdm venv --help
Commands
    create              创建虚拟环境
    list                展示当前项目中创建的虚拟环境
    remove              删除虚拟环境
    activate            打印虚拟环境激活脚本
    purge               清理虚拟环境
triangle@LEARN:~$ pdm use [python] // 当前 pdm 环境使用哪个 python 解释器
triangle@LEARN:~$ pdm venv create [python] [options]
Position
    python                              基于指定的 python 解释器创建虚拟环境，不指定就使用当前 pdm 环境的 python
Options
    --with-pip                          在虚拟环境中安装 pip
    -w [virtualenv|venv|conda|uv]       指定虚拟环境的管理工具
    --name [name]                       将创建全局的虚拟环境，不指定则创建在项目的 '.venv' 文件夹内
triangle@LEARN:~$ pdm run python demo.py // 在虚拟环境中执行命令
Options
    --venv [venv]              指定虚拟环境名，通过 'pdm venv list' 查看
```

### 配置

```term
triangle@LEARN:~$ pdm config // 列出所有配置
    ... 
venv.backend = virtualenv               # 虚拟环境默认方式
venv.in_project = True                  # 在项目本地创建虚拟环境，即 .venv
venv.with_pip = False                   # 虚拟环境中安装 pip 
    ...
pypi.url = https://pypi.org/simple      # 仓库源
pypi.verify_ssl = False                 # 源通信验证，开启可能无法访问仓库
pypi.ignore_stored_index = False
pypi.json_api = False
    ...

```

配置文件分三种类型，从优先级从高到低为
1. `<PROJECT_ROOT>/pdm.toml`: 项目本地配置
1. `<CONFIG_ROOT>/config.toml` : pdm 全局配置
1. `<SITE_CONFIG_ROOT>/config.toml` :  pdm 工具自带的默认配置，不可修改

```term
triangle@LEARN:~$ pdm config --local pypi.verify_ssl False // 修改项目本地配置
triangle@LEARN:~$ pdm config pypi.verify_ssl False // 修改全局配置
```

- `<PROJECT_ROOT>/pdm.toml`

```toml
# 'pdm config --local' 配置
[pypi]
url = 'xxx'
verify_ssl = false
```

- `pyproject.toml` : 除了在 `pdm.toml` 可以对 `pdm` 进行配置，在 `pyproject.toml` 中也能配置

```toml
# 源配置
[[tool.pdm.source]]
name = "private"
url = "https://private.pypi.org/simple"

# 为 'pdm commands' 添加默认的 'options'
[tool.pdm.options]
add = ["--no-isolation", "--no-self"]
install = ["--no-self"]
lock = ["--no-cross-platform"]

# 在锁定文件中排除 'requests' 包的校验
[tool.pdm.resolution]
excludes = ["requests"]
```


### 依赖管理

```term
triangle@LEARN:~$ pdm --help
Commands
    install                 安装项目依赖
    add [package]           往项目添加包
    remove [package]        从项目中移除包
    list                    查看当前项目已经安装成功的包
    show [package]          查看安装的包信息
    search [package]        从镜像仓库中搜索包
    update <package>        更新包
```

pdm 也可以按照使用类型对包进行分组管理`group`：运行依赖包、`dev` 开发依赖包、`test` 测试依赖包以及其他自定义分组。

```term
triangle@LEARN:~$ pdm add --help 
Options 
    -G [group]                      将 package 归类到 group 组，放到 'project.optional-dependencies'
    --dev                           开发依赖包，由 pdm 自行管理，不放到 'project.optional-dependencies'
    -e                              可修改包依赖，例如 git 仓库
triangle@LEARN:~$ pdm install --help
Options
    --with [group]                  需要安装的分组
    --without [group]               不需要安装的分组
    --dev                           需要安装 dev 依赖
    --prod                          不安装 dev 依赖
triangle@LEARN:~$ pdm export -o requirements.txt // 导出依赖
```



### 构建

在 pdm 中并未强制要求用哪个构建工具，可根据 [build](https://github.com/pdm-project/pdm/blob/main/docs/reference/build.md)，在 `pyproject.toml` 中配置自行配置

```toml
[build-system]
requires = ["pdm-backend"]
build-backend = "pdm.backend"
```

### 脚本

pdm 在 `pyproject.toml` 中支持多种类型的脚本配置

```toml
[tool.pdm.scripts]
commond = "flask run -p 54321"
cmd = {cmd = "flask run -p 54321"}
shell = {shell = "cat error.log|grep CRITICAL > critical.log"}
call = {call = "foo_package.bar_module:main"}
```

```term
triangle@LEARN:~$ pdm run call // 执行脚本
```


除了基本的脚本配置外，pdm 也支持一些复杂的脚本配置

- **组合命令**

```toml
[tool.pdm.scripts]
lint = "flake8"
test = "pytest"
all = {composite = ["lint mypackage/", "test -v tests/"]}
# 简化写法
mytask.composite = [
    "echo 'Hello'",
    "echo 'World'"
]
```

- **命令环境配置**

```toml
[tool.pdm.scripts]
start.cmd = "flask run -p 54321"                # start 执行的命令
start.env = {FOO = "bar", FLASK_DEBUG = "1"}    # start 命令的环境变量
start.working_dir = "subdir"                    # 工作目录
```

- **命令传参**

```toml
[tool.pdm.scripts]
cmd = "echo 'xxx {args} xxx'"                   # {args} 会展开传入的参数
test = "echo 'xxx {args:--default} xxx'"        # {args: xxx} 指定默认参数
```

- **前置/后置命令**

```toml
[tool.pdm.scripts]
pre_compress = "{{ Run BEFORE the `compress` script }}" # 前置 pre_xxxx
compress = "tar czvf compressed.tar.gz data/"
post_compress = "{{ Run AFTER the `compress` script }}" # 后置 post_xxxx
```

### 锁定文件

当进行包安装操作时，会生成 `pdm.lock` 锁定文件
- 保证在所有机器上，项目的包都一样
- 便于版本控制，多人协同开发

```term
triangle@LEARN:~$ pdm sync // 根据 pdm.lock 安装包
triangle@LEARN:~$ pdm update // 根据 pyproject.toml 更新 pdm.lock
triangle@LEARN:~$ pdm lock // 生成锁定文件
triangle@LEARN:~$ pdm lock --check // 检查锁定文件
triangle@LEARN:~$ pdm lock --refresh // 刷新锁定文件
```