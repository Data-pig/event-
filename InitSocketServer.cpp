#include "InitSocketServer.h"
#include<atlconv.h>

struct fd_es_set esSet[20] = { 0,{0},{NULL} };
SOCKET socketServer;
extern SOCKET g_sockALL[MAX_CONT];
extern OVERLAPPED g_allOlp[MAX_CONT];
extern int g_count;
extern char str[1024];


//初始化服务器socket	//成功返回1 失败返回0
int InitSocketServer()
{
	//请求操作系统
	SetConsoleCtrlHandler(fun, TRUE);
	//打开网络库
	WORD wdVersion = MAKEWORD(2, 2);//版本号2.2
	WSADATA wdScokMsg;		//LPWSADATA wdScokMsg=malloc(sizeof(WSADATA));
	int nRes = WSAStartup(wdVersion, &wdScokMsg);
	if (0 != nRes)
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("重启下电脑试试，或者检查网络库");
			break;
		case WSAVERNOTSUPPORTED:
			printf("请更新网络库");
			break;
		case WSAEINPROGRESS:
			printf("请重新启动软件");
			break;
		case WSAEPROCLIM:
			printf("请尝试关掉不必要的软件，以为当前网络运行提供充足的资源");
			break;
		}
		return 0;
	}

	//校验版本
	if (2 != HIBYTE(wdScokMsg.wVersion) || 2 != LOBYTE(wdScokMsg.wVersion))
	{
		//版本不对
		//清理网络库
		WSACleanup();
		return 0;
	}

	//AF_INET			方式/地址的类型 ip地址类型ipv4 ipv6
	//SOCK_STREAM		
	socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		int a = WSAGetLastError();
		//清理网络库
		WSACleanup();
		closesocket(socketServer);
		return 0;
	}

	//绑定地址与端口
	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);//用宏转端口号
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//127,本地网络测试 
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*) & si, sizeof(si)))
	{
		//出错了
		int a = WSAGetLastError();
		//释放
		closesocket(socketServer);
		//清理网络库
		WSACleanup();
		return 0;
	}

	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))//监听
	{
		//出错了
		int a = WSAGetLastError();
		//释放
		closesocket(socketServer);
		//清理网络库
		WSACleanup();
		return 0;
	}
	printf("服务器创建成功，等待链接.......\n");

	g_sockALL[g_count] = socketServer;
	g_allOlp[g_count].hEvent = WSACreateEvent();
	g_count++;

	return 1;
}

BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		for (u_int j = 0; j < 20; j++)
		{
			for (u_int i = 0; i < esSet[j].count; i++)
			{
				closesocket(esSet[j].sockall[i]);
				WSACloseEvent(esSet[j].eventall[i]);
			}
		}
		WSACleanup();
		break;
	}
	return TRUE;
}

int PostAccept()
{
	g_sockALL[g_count]= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_allOlp[g_count].hEvent = WSACreateEvent();

	DWORD dwRecvcount;
	BOOL bRes= AcceptEx(g_sockALL[0], g_sockALL[g_count],str,0, sizeof(struct sockaddr_in) + 16,sizeof(struct sockaddr_in) + 16,&dwRecvcount, &g_allOlp[g_count]);
	if (TRUE == bRes)
	{
		//立即完成
		//投递recv
		PostRecv(g_count);
		g_count++;
		PostAccept();
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//延迟处理
			return 0;
		}
		else
		{
			return a;
		}
	}
}
int PostRecv(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = str;
	wsabuf.len = 1024;
	DWORD dwBufferCount;
	DWORD dwFlag=0;
	int nRes=WSARecv(g_sockALL[index],&wsabuf,1, &dwBufferCount,&dwFlag,&g_allOlp[index],NULL);
	if (0 == nRes)
	{
		//立即完成
		printf("%s\n", wsabuf.buf);
		memset(wsabuf.buf,0,1024);
		PostRecv(index);
		return 0;
	}
	else 
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//延迟处理
			return 0;
		}
		else
		{
			return a;
		}
	}
}