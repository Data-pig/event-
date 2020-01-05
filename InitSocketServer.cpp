#include "InitSocketServer.h"
#include<atlconv.h>

struct fd_es_set esSet[20] = { 0,{0},{NULL} };
SOCKET socketServer;
extern SOCKET g_sockALL[MAX_CONT];
extern OVERLAPPED g_allOlp[MAX_CONT];
extern int g_count;
extern char str[1024];


//��ʼ��������socket	//�ɹ�����1 ʧ�ܷ���0
int InitSocketServer()
{
	//�������ϵͳ
	SetConsoleCtrlHandler(fun, TRUE);
	//�������
	WORD wdVersion = MAKEWORD(2, 2);//�汾��2.2
	WSADATA wdScokMsg;		//LPWSADATA wdScokMsg=malloc(sizeof(WSADATA));
	int nRes = WSAStartup(wdVersion, &wdScokMsg);
	if (0 != nRes)
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("�����µ������ԣ����߼�������");
			break;
		case WSAVERNOTSUPPORTED:
			printf("����������");
			break;
		case WSAEINPROGRESS:
			printf("�������������");
			break;
		case WSAEPROCLIM:
			printf("�볢�Թص�����Ҫ���������Ϊ��ǰ���������ṩ�������Դ");
			break;
		}
		return 0;
	}

	//У��汾
	if (2 != HIBYTE(wdScokMsg.wVersion) || 2 != LOBYTE(wdScokMsg.wVersion))
	{
		//�汾����
		//���������
		WSACleanup();
		return 0;
	}

	//AF_INET			��ʽ/��ַ������ ip��ַ����ipv4 ipv6
	//SOCK_STREAM		
	socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		int a = WSAGetLastError();
		//���������
		WSACleanup();
		closesocket(socketServer);
		return 0;
	}

	//�󶨵�ַ��˿�
	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);//�ú�ת�˿ں�
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//127,����������� 
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*) & si, sizeof(si)))
	{
		//������
		int a = WSAGetLastError();
		//�ͷ�
		closesocket(socketServer);
		//���������
		WSACleanup();
		return 0;
	}

	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))//����
	{
		//������
		int a = WSAGetLastError();
		//�ͷ�
		closesocket(socketServer);
		//���������
		WSACleanup();
		return 0;
	}
	printf("�����������ɹ����ȴ�����.......\n");

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
		//�������
		//Ͷ��recv
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
			//�ӳٴ���
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
		//�������
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
			//�ӳٴ���
			return 0;
		}
		else
		{
			return a;
		}
	}
}