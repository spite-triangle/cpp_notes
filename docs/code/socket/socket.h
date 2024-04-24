#ifndef SOCKET_H
#define SOCKET_H

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

#endif /* SOCKET_H */