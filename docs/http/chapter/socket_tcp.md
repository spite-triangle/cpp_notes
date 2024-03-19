# socket TCP

# 介绍


 套接字是一种网络API，提供一种进程间的通信方法，使得相同主机或者不同主机上的进程能够使用socket定义的规范进行双向的数据通信。可以实现TCP与UDP方式的通信。

**socket实现的TCP通讯模式：**
![alt|c,50](../../image/http/socket.png)

# API接口

## server

创建 socket

```cpp
#include <sys/socket.h>

// family：对应的就是AF_INET、AF_INET6等。
// type：套接字类型：SOCK_STREAM、SOCK_DGRAM、SOCK_RAW。
// protocol：0
// 成功：非负套接字描述符。
// 出错：-1
int socket(int family, int type, int proto)
```

绑定服务器地址

```cpp
// sockfd：套接字描述符。
// my_addr：本地地址。可以使用 sockaddr_in 代替
// addrlen：my_addr 长度：
// 成功：0
// 出错：-1
int bind(int sockfd,struct sockaddr *my_addr,int addrlen)

#include <arpa/inet.h>
struct sockaddr_in {
    // AF_INET、AF_INET6等
    short int sin_family;         
    // htons(端口号)
    unsigned short int sin_port;  
    // inet_pton(AF_INET,"ip",&sin_addr) 
    struct in_addr  sin_addr;     
};
```

服务器启动客户端监听服务

```cpp
// sockfd：套接字描述符
// backlog：请求队列中允许的最大请求数，大多数系统默认为20
// 成功：0
// 出错：-1
int listen(int sockfd,int backlog)
```

服务器连接客户端：
- **请求队列已满时，该服务器套接字将拒绝连接请求**
- **请求队列为空时，进程进入睡眠（阻塞），直到新的连接到达**

```cpp
// sockfd：套接字描述符
// addr：客户端地址
// addrlen：地址长度
// 成功：0
// 出错：-1
int accept(int sockfd,struct sockaddr * addr,socklen_t* addrlen)
```

## client

连接服务器

```cpp
// sockfd：套接字描述符
// serv_addr：服务器端地址
// addrlen：地址长度
// 返回值：
// 成功：0
// 出错：-1
int connect(int sockfd,struct sockaddr* serv_addr,int addrlen)
```

## 读写

```cpp
// sockfd：套接字描述符
// msg：指向要发送数据的指针
// len：数据长度
// flags：一般为0
// 返回值：
// 成功：发送的字节数
// 出错：-1
int send(int sockfd,const void* msg,int len,int flags)

// sockfd：套接字描述符
// buf：存放接受数据的缓冲区
// len：数据长度
// flags：一般为0
// 返回值：
// 成功：接受的字节数
// 出错：-1
int recv(int sockfd,void* buf,int len,unsigned int flags)

#include <unistd.h>
int write(int sockfd, char *buf, int buf_len);

// 数据在不超过指定的长度的时候有多少读多少，没有数据则会一直等待
int read(int sockfd, char *buf, int buf_len); 

// UDP 协议使用
int sendto(int sockfd,const void* msg,int len,unsigned int flags,const struct sockaddr* to,int tolen)
int recvfrom(int sockfd,void * buf,int len,unsigned int flags,struct sockaddr* from,int* fromlen)
```

**flags**:
- MSG_DONTROUTE：不查找路由表
- MSG_OOB：接受或发送带外数据
- MSG_PEEK：查看数据,并不从系统缓冲区移走数据
- MSG_WAITALL：阻塞模式，等待数据凑够 `len` 的长度，但是也不会一直等着
- MSG_DONTWAIT：非阻塞模式，不等待，读取一段就拿走，就算总长度小于`len`
- 0：两种方式等价


## 关闭

```cpp
#include <unistd.h>
int close(int sockfd);     
```

# 附录

## 封装 socket 组件


<details>
<summary><span class="details-title">封装组件</span></summary>
<div class="details-content"> 

 
- **头文件**

```cpp
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include<stdio.h>
#include<stdbool.h>
#include<WinSock2.h> // windows平台网络库头文件
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // 库文件

// 客户端信息结构体
struct SockInfo {
	sockaddr_in addr;
	int fd;
};

// 封装错误提示
#define err(errMsg) printf("[line:%d] %s failed code %d\n", __LINE__, errMsg, WSAGetLastError());

// 初始化 socket 网络
bool InitSocketNet();

// 关闭 socket 网络
bool CloseSocketNet();

// 创建 socket
SOCKET InitSocket();

// 关闭
int CloseSocket(SOCKET fd);

// 创建服务
SOCKET InitServer(const char * ip, u_short port);

// 接收客户端
SockInfo AcceptClient(SOCKET fd);

// 连接服务
int ConnectServer(SOCKET fd, const char * ip, u_short port);

// 发送消息
bool SendMsg(int fd, const char* buf, int len);

// 接收消息
bool RecvMsg(int fd, char* buf, int bufSize);

#endif /* SOCKET_HPP */
```

- **源文件**

```cpp
#include "socket.h"

bool InitSocketNet()
{
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		err("WSAStartup");
		return false;
	}
	return true;
}

bool CloseSocketNet()
{
	if (0 != WSACleanup()) {
		err("WSACleanup");
		return false;
	}
	return true;
}

SOCKET InitSocket()
{
    // 1. 创建空 socket
	// param1: 地址协议族 ipv4 ipv6
	// param2: 传输协议类型 流式套接字 数据报
	// param3: 使用具体的某个传输协议
	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1)
	{
		err("socket");
		return -1;
	}
    return fd;
}

int CloseSocket(SOCKET fd)
{
    return closesocket(fd);
}

// 发送消息
bool SendMsg(int fd, const char* buf, int len) {
	int ret = send(fd, buf, len, 0);
	if (ret == -1) {
		err("send msg");
		return false;
	}
	return true;
}

// 接收消息
bool RecvMsg(int fd, char* buf, int totalLen) {
    int recvLen = 0;
    const int readMax = 1024; 

    while (recvLen < totalLen)
    {
        int buffLen = (totalLen - recvLen) > readMax ? readMax : totalLen - recvLen;
        int len = recv(fd, buf + recvLen, buffLen, 0);

        if (len > 0) {
            recvLen += len;
        }
        else if (len == 0)
        {
            return false;
        }
        else {
            perror("recv");
            return false;
        }

    }
    return true;
}

SOCKET InitServer(const char * ip, u_short port)
{
    // 1. 创建空 socket
	SOCKET fd = InitSocket();
	if (fd == -1)
	{
		err("socket");
		return -1;
	}

	// 2. 给socket绑定ip地址和端口号
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &saddr.sin_addr);

	int ret = bind(fd, (sockaddr*)&saddr, sizeof(saddr));
	if (ret == -1) {
		err("bind");
		return -1;
	}

	//3. 监听
	ret = listen(fd, 10);
	if (ret == -1)
	{
		err("listen");
		return -1;
	}
	printf("等待客户端连接...\n");
    
    return fd;
}

SockInfo AcceptClient(SOCKET fd)
{
    SockInfo client;
	int addrLen = sizeof(sockaddr_in);
    client.fd = accept(fd, (struct sockaddr*)&client.addr, &addrLen);
    return client;
}

int ConnectServer(SOCKET fd, const char * ip, u_short port)
{
	// 2. 与服务器建立连接
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &addr.sin_addr);
	
	int ret = connect(fd, (sockaddr*)&addr, sizeof(addr));
	if ( ret == -1)
	{
		err("connect");
		return -1;
	}
    return ret;
}

```


</div>
</details>


## 使用

- **服务**

```cpp
void main(){
    // 初始化网络库
	InitSocketNet();

    // 初始化服务
	SOCKET fd = InitServer("127.0.0.1", 2333);

    // 接收客户端
    SockInfo client = AcceptClient(fd);

    char buff[1024] = {0};
    RecvMsg(client.fd, buff, sizeof(buff));
    printf(buff);
    // SendMsg(client.fd, buf);

    closeSocket(client.fd);
    closeSocket(fd);
    CloseSocketNet();
}
```

- **客户端**

```cpp

void main(){
    InitSocketNet();

    auto fd = InitSocket();

    ConnectServer(fd, "127.0.0.1", 2333);

    const char * buff = "fuck you";
    SendMsg(fd, buff, strlen(buff));

    Sleep(1000);

    closeSocket(fd);

    CloseSocketNet();
}
```

