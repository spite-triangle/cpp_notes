# http 协议

# 1. 基本概念

## 1.1. 简介

HTTP协议(超文本传输协议HyperText Transfer Protocol)，它是基于TCP协议的应用层传输协议，简单来说就是客户端和服务端进行数据传输的一种规则。并且http是一种无状态 (stateless) 协议, HTTP协议本身不会对发送过的请求和相应的通信状态进行持久化处理。

HTTP协议的实际过程：
- **请求**: 客户端发送给服务器
- **响应**：服务器接收客户端的请求，并处理

## 1.2. 请求报文

<p style="text-align:center;"><img src="/cpp_notes/image/http/requestionMessage.png" width="50%" align="middle" /></p>

报文结构：
- 请求行
    - 请求方法: GET、POST、DELETE、PUT ...
    - 请求路径
    - 协议：`HTTP/1.1`
- 请求头
- **空行**：<span style="color:red;font-weight:bold"> 空两行 </span>
- 请求主体

## 1.3. 响应报文

<p style="text-align:center;"><img src="/cpp_notes/image/http/responseMessage.png" width="50%" align="middle" /></p>

报文结构：
- 响应行
    - 版本协议
    - 状态码
    - 状态文字
- 响应头信息
- **空行**: <span style="color:red;font-weight:bold"> 空两行 </span>
- 响应主体信息

# 2. 请求方法

## 2.1. 测试环境

测试请求头：
- 服务器：利用 [phpstudy](https://www.xp.cn/download.html) 快速搭建一个本地服务器 <span style="color:red;font-weight:bold"> (2018 旧版比较稳定) </span>
- 客户端：利用 `telnet` 进行远程连接。`ctrl + ]` 显示打印信息

## 2.2. GET

**作用：** 向服务器请求数据，并获取服务端数据

```term
triangle@LEARN:~$ telnet localhost 80 // 远程连接工具

# 请求
GET /index.php HTTP/1.1
Host: localhost


# 响应
HTTP/1.1 200 OK
Date: Tue, 30 Aug 2022 03:55:34 GMT
Server: Apache/2.4.23 (Win32) OpenSSL/1.0.2j PHP/5.4.45
X-Powered-By: PHP/5.4.45
Content-Length: 11
Content-Type: text/html

Hello World
```

## 2.3. POST

**作用：** 就是发送、提交。向服务器提交/发送要被处理的数据。

```term
triangle@LEARN:~$ telnet localhost 80

POST /testPost.php HTTP/1.1
Host: localhost
Content-type: application/x-www-form-urlencoded // 请求主体的类型
Content-length: 10 // 请求主体的长度
// 第一空行
// 第二空行
us=12

HTTP/1.1 200 OK
Date: Tue, 30 Aug 2022 05:50:30 GMT
Server: Apache/2.4.23 (Win32) OpenSSL/1.0.2j PHP/5.4.45
X-Powered-By: PHP/5.4.45
Content-Length: 10
Content-Type: text/html
```

## 2.4. HEAD

**作用：** 向服务器确认数据是否存在，不要返回内容

```term
triangle@LEARN:~$ telnet localhost 80

// 确认网站还在不在
HEAD / HTTP/1.1
Host: localhost

HTTP/1.1 200 OK
Date: Tue, 30 Aug 2022 06:13:58 GMT
Server: Apache/2.4.23 (Win32) OpenSSL/1.0.2j PHP/5.4.45
X-Powered-By: PHP/5.4.45
Content-Type: text/html
```

## 2.5. 其他方法

> [!tip]
> 这些方法，服务器可能不支持

- **PUT**：往服务器上的文件中写入内容
- **TRACE**：用代理上网时，查看代理是否修改了原始的 HTTP 请求
- **OPTIONS**：返回服务器支持哪些方法

# 3. 状态码

<p style="text-align:center;"><img src="/cpp_notes/image/http/httpCode.png" width="50%" align="middle" /></p>

<div style="margin: 0 auto;padding:1rem;width: 30rem;">

| 状态码 | 状态文字     | 功能           |
| ------ | ------------ | -------------- |
| 200    | OK           | 没问题         |
| 304    | Not Modified | 在缓存中没修改 |

</div>

