#pragma once
#include<WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
#include<iostream>
#include <mswsock.h>

#define MAX_CONT 1024

BOOL WINAPI fun(DWORD dwCtrlType);

//��ʼ��������socket	//�ɹ�����1 ʧ�ܷ���0
int InitSocketServer();
int PostRecv(SOCKET socketClient);

typedef struct fd_es_set
{
	u_int count;
	SOCKET sockall[64];//64��
	WSAEVENT eventall[64];//64
};



