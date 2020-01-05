#pragma once
#include<WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
#include<iostream>
#include <mswsock.h>

#define MAX_CONT 1024

BOOL WINAPI fun(DWORD dwCtrlType);

//初始化服务器socket	//成功返回1 失败返回0
int InitSocketServer();
int PostRecv(SOCKET socketClient);

typedef struct fd_es_set
{
	u_int count;
	SOCKET sockall[64];//64个
	WSAEVENT eventall[64];//64
};



