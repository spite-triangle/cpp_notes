# shell

# 概念

![shell|c,40](./../../image/operationSystem/shell.jpg)

- **kernal** : 提供操作系统的系统调用
- **shell** : 与人交互的程序，用户客户通过 shell 来使用操作系统的 kernal

```term
triangle@LEARN:~$ main sh // 最简单的 linux shell 文档
```

# xv6 shell

## 编译

> - [shell](http://jyywiki.cn/pages/OS/2022/demos/sh-xv6.c) 
> - [back shell](../../example/operateSystem/sh-xv6.c) 

程序编译：

```term
triangle@LEARN:~$ gcc -c -ffreestanding sh-xv6.c -g -o2 // -ffreestanding 零依赖，包含系统库
triangle@LEARN:~$ ld sh-xv6.o -o sh
triangle@LEARN:~$ ./sh
> /bin/pwd
/home/triangle
```

## cd

切换路径，只能靠 shell 自己利用系统调用完成，其余的命令行都是靠对应程序实现。

```cpp
void _start() {
  static char buf[100];

  // 循环等待用户输出命令行
  while (getcmd(buf, sizeof(buf)) >= 0) {

    // 判断是否为 cd 命令
    if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf) - 1] = 0;  // chop \n
      if (syscall(SYS_chdir, buf + 3) < 0) print("cannot cd ", buf + 3, "\n", NULL);
      continue;
    }

    // 创建一条子进程，用于命令行执行
    if (syscall(SYS_fork) == 0) runcmd(parsecmd(buf));

    // 主线程等待
    syscall(SYS_wait4, -1, 0, 0, 0);
  }
  syscall(SYS_exit, 0);
}
```

## 命令行解析

shell 会将用户输入的命令行解析为语法树，然后根据语法树执行命令。

```term
triangle@LEARN:~$ (/bin/echo hello > test.txt ; /bin/cat test.txt) | /bin/wc -l
```

上述指令通过语法树可以解析为：

![expression tree|c,55](../../image/operationSystem/expressionTree.jpg)


<details>
<summary><span class="details-title">语法树</span></summary>
<div class="details-content"> 

```cpp
/* 生成语法树 */
struct cmd* parsecmd(char* s) {
  char* es;
  struct cmd* cmd;

  es = s + strlen(s);
  // 解析命令行
  cmd = parseline(&s, es);

  // 查询 s[0] 是否到了指定字符，中间会过滤掉空白符
  peek(&s, es, "");
  assert(s == es);

  // 遍历语法树，初始化值
  nulterminate(cmd);
  return cmd;
}

struct cmd* parseline(char** ps, char* es) {
  struct cmd* cmd;
  // 根据管道符拆分命令行
  //  pipecmd
  //    /  \
  // Lcmds  Rcmds
  cmd = parsepipe(ps, es);

  // 解析后台运行命令
  //     backcmd
  //        |
  //       cmds 
  while (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }

  // 依次执行命令
  //      listcmd
  //       /  \
  //   Fronts  Backs
  if (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  return cmd;
}

struct cmd* parsepipe(char** ps, char* es) {
  struct cmd* cmd;
  
  // 有括号，先去除括号；
  // 无括号，解析出 execcmd
  cmd = parseexec(ps, es);

  //  pipecmd
  //    /  \
  // Lcmds  Rcmds
  if (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd* parseblock(char** ps, char* es) {
  struct cmd* cmd;
  
  // 去除左括号
  assert(peek(ps, es, "("));
  gettoken(ps, es, 0, 0);
  
  // 解析命令树
  cmd = parseline(ps, es);
  assert(peek(ps, es, ")"));

  // 去除右括号
  gettoken(ps, es, 0, 0);

  // 解析重定向
  //  redircmd
  //     |
  //    cmds
  cmd = parseredirs(cmd, ps, es);
  return cmd;
}

struct cmd* parseredirs(struct cmd* cmd, char** ps, char* es) {
  int tok;
  char *q, *eq;

  // 解析重定向
  //  redircmd
  //     |
  //    cmds
  while (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    assert(gettoken(ps, es, &q, &eq) == 'a');
    switch (tok) {
      case '<':
        cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
        break;
      case '>':
        cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_TRUNC, 1);
        break;
      case '+':  // >>
        cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT, 1);
        break;
    }
  }
  return cmd;
}

/* 语法树节点 */
struct cmd* execcmd(void) {
  struct execcmd* cmd;
  cmd = zalloc(sizeof(*cmd));
  cmd->type = EXEC;
  return (struct cmd*)cmd;
}

struct cmd* redircmd(struct cmd* subcmd, char* file, char* efile, int mode,
                     int fd) {
  struct redircmd* cmd;
  cmd = zalloc(sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd* pipecmd(struct cmd* left, struct cmd* right) {
  struct pipecmd* cmd;
  cmd = zalloc(sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd* listcmd(struct cmd* left, struct cmd* right) {
  struct listcmd* cmd;
  cmd = zalloc(sizeof(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd* backcmd(struct cmd* subcmd) {
  struct backcmd* cmd;
  cmd = zalloc(sizeof(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  return (struct cmd*)cmd;
}
```
</div>
</details>


## 命令行执行

- 单命令: shell 进程创建一条子进程，然后子进程执行 execve 系统调用

```cpp
void _start() {
  static char buf[100]; 

  // 循环等待用户输出命令行
  while (getcmd(buf, sizeof(buf)) >= 0) {
    ...
    // 创建一条子进程，用于命令行执行
    if (syscall(SYS_fork) == 0) runcmd(parsecmd(buf));
    // 主线程等待
    syscall(SYS_wait4, -1, 0, 0, 0);
  }
  syscall(SYS_exit, 0);
}

void runcmd(struct cmd* cmd) {
  ...
  switch (cmd->type) {
    case EXEC:
      ecmd = (struct execcmd*)cmd;
      if (ecmd->argv[0] == 0) syscall(SYS_exit, 1);
      // 子进程执行程序 
      long ret = syscall(SYS_execve, ecmd->argv[0], ecmd->argv, NULL);
      print("fail to exec ", ecmd->argv[0], "\n",NULL);
      break;
      ....
  }
}
```

- 多行命令

```cpp 
void runcmd(struct cmd* cmd) {
  ...
    case LIST:
        lcmd = (struct listcmd*)cmd;
        // 生成一个子进程执行左边的节点
        if (syscall(SYS_fork) == 0) runcmd(lcmd->left);
        // 等待左边执行完
        syscall(SYS_wait4, -1, 0, 0, 0);
        // 执行右边的节点
        runcmd(lcmd->right);
        break;
  ...
}
```
- 管道

```cpp
void runcmd(struct cmd* cmd) {
    ...
    case PIPE:
      pcmd = (struct pipecmd*)cmd;
      assert(syscall(SYS_pipe, p) >= 0);
      if (syscall(SYS_fork) == 0) {
        // 关闭当前进程标准输出
        syscall(SYS_close, 1);
        // 当前进程标准输出绑定到管道
        syscall(SYS_dup, p[1]);
        // 关闭当前进程持有的管道输入与输出 
        syscall(SYS_close, p[0]);
        syscall(SYS_close, p[1]);
        // 运行左边，执行 execve 之后就不会返回
        runcmd(pcmd->left);
      }
      if (syscall(SYS_fork) == 0) {
        // 关闭当前进程标准输入
        syscall(SYS_close, 0);
        // 当前进程标准输入绑定到管道
        syscall(SYS_dup, p[0]);
        // 关闭当前进程持有的管道输入与输出 
        syscall(SYS_close, p[0]);
        syscall(SYS_close, p[1]);
        // 运行右边，执行 execve 之后就不会返回
        runcmd(pcmd->right);
      }
      // 主进关闭管道的输入与输出
      syscall(SYS_close, p[0]);
      syscall(SYS_close, p[1]);
      // 等待子进程运行完毕
      syscall(SYS_wait4, -1, 0, 0, 0);
      syscall(SYS_wait4, -1, 0, 0, 0);
      break;
    ...
}
```

- 重定向

```cpp
void runcmd(struct cmd* cmd){
    ...
    case REDIR:
      rcmd = (struct redircmd*)cmd;
      syscall(SYS_close, rcmd->fd);
      if (syscall(SYS_open, rcmd->file, rcmd->mode, 0644) < 0) {
        print("fail to open ", rcmd->file, "\n", NULL);
        syscall(SYS_exit, 1);
      }
      runcmd(rcmd->cmd);
      break;
    ...
}
```
