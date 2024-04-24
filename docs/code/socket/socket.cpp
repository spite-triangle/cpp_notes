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
    return fd;
}

SockInfo AcceptClient(SOCKET fd)
{
	printf("等待客户端连接...\n");
    SockInfo client;
	int addrLen = sizeof(sockaddr_in);
    client.fd = accept(fd, (struct sockaddr*)&client.addr, &addrLen);

    char ip[32];
    printf("客户端fd: %d, IP：%s, 端口:%d\n",
        client.fd,
        inet_ntop(AF_INET, &client.addr.sin_addr.S_un, ip, sizeof(ip)),
        ntohs(client.addr.sin_port)
    );
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