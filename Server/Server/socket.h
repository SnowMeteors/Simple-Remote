#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

/**
 *
 * socket��صĲ���
 * ��
 * 2022/3/8
 *
**/

// ��ʼ��sock
void InitSock();
// �����˿�
SOCKET Listen(int port);
// ��������
SOCKET Connect(SOCKET sClient);
// ��������
void SendData(SOCKET sServer, char buffer[], int size);
// ��������
void RecvData(SOCKET sServer, char buffer[], int size);
// ����ʧ����Ϣ
void SendError(SOCKET sServer);
// ���ͳɹ���Ϣ
void SendSuccess(SOCKET sServer);
// �Ƿ���յ�������Ϣ
BOOL IsRecvError(SOCKET sServer);