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

// ��������
SOCKET Connect(char* ip, int port)
{
	int ret;

	SOCKET sServer = socket(AF_INET, SOCK_STREAM, 0);
	if (sServer == INVALID_SOCKET)
	{
		printf("socket fail\n");
		exit(-1);
	}

	// ���÷��͵�ַ
	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = inet_addr(ip);
	printf("          =[ connect to %-15s%-5d                    ]\n\n", inet_ntoa(server.sin_addr), htons(server.sin_port));

	// ��������
	ret = connect(sServer, (SOCKADDR*)&server, sizeof(SOCKADDR));
	if (ret == SOCKET_ERROR)
	{
		printf("connect fail\n");
		exit(-1);
	}
	return sServer;
}

// ��������
void SendData(SOCKET sServer, char buffer[], int size)
{
	// ��ֹ�Է���û�򿪽���ͨ���ͷ�����
	Sleep(500);

	int ret = send(sServer, buffer, size, 0);
	if (ret == SOCKET_ERROR || ret == 0)
	{
		printf("send fail\n");
		exit(-1);
	}
}

// ��������
void RecvData(SOCKET sServer, char* buffer, int size)
{
	int ret = recv(sServer, buffer, size, 0);
	if (ret == 0 || ret == SOCKET_ERROR)
	{
		printf("recv fail\n");
		exit(-1);
	}
}

