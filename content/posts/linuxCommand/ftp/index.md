---
title: "FTP"
date: "2025-04-07"
draft: false
description: "介绍 FTP 文件传输协议、vsftpd 配置安装及匿名/本地/虚拟用户登录方式"
weight: 5
tags: ["FTP", "vsftpd", "文件传输", "sftp"]
categories: ["Linux", "网络服务"]
---


# 基本概念

FTP (File Transfer Protocol)，文件传输协议，用于在网络中进行文件传输，属于应用层且基于TCP协议实现，允许用户以文件操作的方式（如文件的增、删、改、查、传送等）与另一主机相互通信。在 linux 上 vsftpd 软件实现了FTP 服务协议。
- 默认20端口：数据传输
- 默认21端口：接收客户端的FTP指令

FTP的工作模式分为两种：
- 主动模式：服务器主动向客户端发起请求
- 被动模式：服务器等待客户端发起请求

# vsftpd

## 配置安装

- 安装

```term
$ sudo apt install vsftpd
```

- 配置

```term
$ cat /etc/vsftpd.conf
# 允许匿名用户登录
anonymous_enable=YES
# 允许匿名用户上传数据
anon_upload_enable=YES
# 匿名用户可写
anon_mkdir_write_enable=YES
# 匿名用户可修改
anon_other_write_enable=YES
# 本地用户登录
local_enable=YES
# 可写
write_enable=YES
# 文件权限
local_umask=022
# 显示目录信息
dirmessage_enable=YES
# 配置日志路径
xferlog_file=/var/log/xferlog
# 启动 20 端口
connect_from_port_20=YES
# 启动本地 127.0.0.1 监听 
listen=YES
# 启动 ipv6 监听
listen_ipv6=YES
# 启用用户权限配置文件
userlist_enable=YES
userlist_deny=YES
$ grep -Ev '^#' /etc/vsftpd.conf  # 过滤注释行
```

- 启用配置

```term
$ systemctl restart vsftpd
```

## 登录

> [!note]
> 登录模式有三种：
> - 本地用户模式：通过 linux 的账号密码进行登录
> - 匿名用户模式：不需要账号密码直接访问
> - 虚拟用户模式：为FTP创建用户数据库，验证账号密码


- 匿名用户登录

```term
$ ftp 196.124.1.250
# 匿名用户
Name: anonymous
# 不要密码
Password:
login successful.
ftp> ls
pub
ftp> ? # 指令帮助文档
```

登录 ftp 后会存在无法写的问题，这是因为客户端连接到服务器的路径为 `/var/ftp`。这个文件夹的权限并未向其他用户开放，因此客户端无法写入

```term
$ ll
total 0
drwxr-xr-x 1 root root 4096 Feb 28 09:41 ./
drwxr-xr-x 1 root root 4096 Feb 28 09:41 ../
drwxr-xr-x 1 root root 4096 Feb 28 09:41 pub/
$ sudo chown -R ftp.ftp pub # 修改权限
$ ll
total 0
drwxr-xr-x 1 root root 4096 Feb 28 09:41 ./
drwxr-xr-x 1 root root 4096 Feb 28 09:41 ../
drwxr-xr-x 1 triangle triangle 4096 Feb 28 09:41 pub/
```

- 本地用户登录

```term
$ ftp 196.124.1.250
Name: linux 用户
Password: linux 用户密码
login successful.
ftp> 
```

本地用户登录后，客户端连接到服务器的路径为 linux 用户的家目录。

```term
$ vim /etc/vsftpd/ftpusers # 用户登录黑名单
```

# sftp

SFTP（Secure File Transfer Protocol)，安全文件传输协议，是一种基于可靠数据流（data stream），提供文件存取和管理的网络传输协议。与 FTP 协议相比，SFTP 在客户端与服务器间提供了一种更为安全的文件传输方式，因此 SFTP 的性能就弱于 FTP。在 linux 上，SFTP 在 openssl 中实现，因此 SFTP 的安全性保障也使用了 ssh 协议。

```term
$ vim /etc/ssh/sshd_config # 可以对 sftp 进行配置
# 强制执行内部sftp
Subsystem       sftp    internal-sftp
# 限定只有 GroupName 组下面的用户能访问
Match Group GroupName  
# 设定用户组访问的根目录，属主和属组必须是root，且最大权限只能为 755
ChrootDirectory /sftp/%u    
# 强制执行这里指定的命令而忽略客户端提供的任何命令
ForceCommand    internal-sftp    
# 是否允许TCP转发
AllowTcpForwarding no
# 是否允许进行 X11 转发  
X11Forwarding no 
$ systemctl restart sshd
```

连接 sftp

```term
$ sftp user@192.168.182.172 
sftp> help # 查看所有指令
```
