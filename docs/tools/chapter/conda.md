# Conda

# anaconda


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


# miniforge

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

4. miniforge 的 `base` 没有安装 python，需要自己手动安装

```term
(base) triangle@LEARN:~$ conda install python
```

# mamba

>[!tip]
> `miniforge` 自带 `mamba` ，无需安装


`mamba` 是 `conda` 的下载加速版，用法基本同 `conda`。


```term
triangle@LEARN:~$ conda install -n base conda-libmamba-solver
triangle@LEARN:~$ conda config --set solver libmamba
triangle@LEARN:~$ conda install mamba -n base -c conda-forge // 安装 mamba
```

