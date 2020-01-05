#include<WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
#include<iostream>
#include<windows.h>
#include<string>
#include "InitSocketServer.h"


//定义变量
extern SOCKET socketServer;
extern struct fd_es_set esSet[20]; 
char str[1024] = {0};

SOCKET g_sockALL[MAX_CONT];
OVERLAPPED g_allOlp[MAX_CONT];
int g_count ;

int main()
{
	if (0 == InitSocketServer())  
	{
		WSACleanup();
	}
	

	closesocket(socketServer);
	return 0;
}



