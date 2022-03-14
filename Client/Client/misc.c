#include "misc.h"

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
	if (strcmp(buffer, "error") == 0)
	{
		return TRUE;
	}
	return FALSE;
}

// INTתCHAR *
void IntToStr(int num, char* str)
{
	itoa(num, str, 10);
}

// TCHARתCHAR *
void TcharToStr(const TCHAR* tchar, char* _char)
{
	int iLength;
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

// LPWSTRתCHAR *
void LpwstrToStr(LPWSTR lpwstr, char* str)
{
	int num = WideCharToMultiByte(CP_OEMCP, (DWORD)NULL, lpwstr, -1, NULL, 0, NULL, FALSE);
	WideCharToMultiByte(CP_OEMCP, (DWORD)NULL, lpwstr, -1, str, num, NULL, FALSE);
}

// ����a-b�������
int randomNumber(int a, int b)
{
	int random;
	random = rand() % (b - a + 1) + a;
	return random ;
}

// CHAR *תLPCWSTR
void StrToLpcwstr(char* str, LPCWSTR lpcwstr)
{
	memset(lpcwstr, 0, sizeof(lpcwstr));
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, lpcwstr, MAX_PATH);
}

// ������������ض��ַ�
int Left(char buffer[], char chr)
{
	int index = -1;
	int len = strlen(buffer);
	for (int i = len - 1; i >= 0; i--)
	{
		if (buffer[i] == chr)
		{
			index = i;
			break;
		}
	}

	return index;
}

// �������Ҳ����ض��ַ�
int Right(char buffer[], char chr)
{
	int index = -1;
	int len = strlen(buffer);

	for (int i = 0; i < len; i++)
	{
		if (buffer[i] == chr)
		{
			index = i;
			break;
		}
	}
	return index;
}

// ��ȡip��ַ
void GetIP(char src[], char dest[], int index,int size)
{
	strncpy(src, dest + size + 1, index - size -1);
}

// ��ȡ�˿�
void GetPort(char src[], char dest[], int index,int size)
{
	strncpy(src, dest + index + 1, strlen(dest) - index);
}

// ���nc����msf�����Ƿ񰴸�ʽ����
BOOL IsErrorFormat(char cmd[], int index,int size)
{
	if (cmd[index - 1] == ' ' || index <= size)
	{
		return TRUE;
	}
	return FALSE;
}
