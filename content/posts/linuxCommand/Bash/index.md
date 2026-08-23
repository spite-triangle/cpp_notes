---
title: "Bash"
date: "2025-04-07"
draft: false
description: "介绍 Bash 基本概念、命令展开、别名设置与常用快捷键"
weight: 1
tags: ["Bash", "shell脚本", "命令行"]
categories: ["Linux", "命令行工具"]
---


# 基本概念

Bash 是一个命令处理器，运行在文本窗口中，可以用来执行用户输入的指令，例如用户在 Linux 终端中输入的指令，就是靠 Bash 翻译文本，然后去执行对应的程序。将多个指令写到一个文本文件中，让 Bash 批量解析运行，这就是 shell 脚本。

```term
$ sh hello.sh # bash 执行脚本
```

# 展开

Bash 可以通过 `{}` 进行命令展开

```term
$ echo fuck{1,2,3,4} 
fuck1 fuck2 fuck3 fuck4
$ echo fuck{1..4} # .. 可以指定范围
fuck1 fuck2 fuck3 fuck4
$ echo fuck{1..4..2} # {开始..结束..步长}
fuck1 fuck3
```

# alias

Bash 允许给指令设置一个别名

```term
$ alias # 查看别名
alias l='ls -CF'
alias la='ls -A'
alias ll='ls -alF'
$ alias rm='rm -i' # 自定义别名
$ unalias rm # 删除别名
```

# 快捷键

![快捷键](./resources/terminalKeys.png)

- cltr + L ：清屏，相当于 `clear`
- tab : 查找 `$path` 路径下的程序名，进行补全


