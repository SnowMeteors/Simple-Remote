#pragma once
#include "socket.h"

/**
 *
 * ���������Ӱ˵ĺ���
 * ��
 * 2022/3/11
 *
**/

// ����ʧ����Ϣ
void SendError(SOCKET sServer);
// ���ͳɹ���Ϣ
void SendSuccess(SOCKET sServer);
// �Ƿ���յ�������Ϣ
BOOL IsRecvError(SOCKET sServer);
// INTתCHAR *
void IntToStr(int num, char* str);
// TCHARתCHAR *
void TcharToStr(const TCHAR* tchar, char* _char);
// CHAR *תLPCWSTR
void StrToLpcwstr(char* str, LPCWSTR lpcwstr);
// LPWSTRתCHAR *
void LpwstrToStr(LPWSTR lpwstr, char* str);
// ������������ض��ַ�
int Left(char buffer[], char chr);
// �������Ҳ����ض��ַ�
int Right(char buffer[], char chr);
// ����a-b�������
int randomNumber(int a, int b);
// ��ȡip��ַ
void GetIP(char src[], char dest[], int index, int size);
// ��ȡ�˿�
void GetPort(char src[], char dest[], int index, int size);
// ���ip�Ͷ˿��Ƿ񰴸�ʽ����
BOOL IsErrorFormat(char cmd[], int index,int size);

