#pragma once
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


/**
 * 
 * socket��صĲ���
 * ��
 * 2022/3/8
 * 
**/

// ��ʼ��socket
void InitSock();
// ��������
SOCKET Connect(char* ip, int port);
// ��������
void SendData(SOCKET sServer, char buffer[], int size);
// ��������
void RecvData(SOCKET sServer, char* buffer, int size);