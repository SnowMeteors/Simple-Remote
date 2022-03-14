#include "socket.h"

// ��ʼ��sock
void InitSock()
{
	WSADATA wsa;
	int ret;
	// ��ʼ��socket ʹ��2.2�汾 �ɹ�����0
	ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (ret)
	{
		printf("WSAStartup fail\n");
		exit(-1);
	}
}

// �����˿�
SOCKET Listen(int port)
{
	int ret;

	// ����socket
	SOCKET sClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sClient == INVALID_SOCKET)
	{
		printf("socket fail\n");
		exit(-1);
	}

	// ��������
	SOCKADDR_IN local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	// ��������ip����
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// �󶨶˿�
	ret = bind(sClient, (SOCKADDR*)&local, sizeof(SOCKADDR));
	if (ret == SOCKET_ERROR)
	{
		printf("bind fail\n");
		exit(-1);
	}

	// ��������
	ret = listen(sClient, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("listen fail\n");
		exit(-1);
	}

	return sClient;
}

// ��������
SOCKET Connect(SOCKET sClient)
{
	// ���������������
	SOCKADDR_IN server;
	int AddrSize = sizeof(SOCKADDR);
	SOCKET sServer = accept(sClient, (SOCKADDR*)&server, &AddrSize);
	if (sServer == INVALID_SOCKET)
	{
		printf("accept fail\n");
		return 0;
	}
	char ip[20];
	inet_ntop(AF_INET, (SOCKADDR_IN*)&server.sin_addr, ip, 16);
	printf("          =[ connect from %-15s%-5d                  ]\n\n", ip, ntohs(server.sin_port));
	//printf("connect from %s:%d\n\n", ip, ntohs(server.sin_port));

	return sServer;
}

// ��������
void SendData(SOCKET sServer, char buffer[], int size)
{
	// ��ֹ�Է���û�򿪽���ͨ���ͷ�����
	Sleep(10);

	int ret;
	ret = send(sServer, buffer, size, 0);
	if (ret == 0 || ret == SOCKET_ERROR)
	{
		printf("send fail");
		exit(-1);
	}
}

// ��������
void RecvData(SOCKET sServer, char buffer[], int size)
{
	int ret = recv(sServer, buffer, size, 0);
	if (ret == 0 || ret == SOCKET_ERROR)
	{
		printf("recv fail\n");
		exit(-1);
	}
}

// ���ʹ�����Ϣ
void SendError(SOCKET sServer)
{
	char buffer[] = "error";
	SendData(sServer, buffer, sizeof(buffer));
}

// ���ͳɹ���Ϣ
void SendSuccess(SOCKET sServer)
{
	char buffer[] = "success";
	SendData(sServer, buffer, sizeof(buffer));
}

// �Ƿ���յ�������Ϣ
BOOL IsRecvError(SOCKET sServer)
{
	char buffer[20] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));
	RecvData(sServer, buffer, sizeof(buffer));
	if (strcmp(buffer,"error") == 0)
	{
		return TRUE;
	}
	return FALSE;
}