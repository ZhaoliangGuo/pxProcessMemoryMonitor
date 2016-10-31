#include "stdafx.h"
#include "pxMemoryManager.h"

//��������Ȩ��
bool AdjustPurview()
{
	TOKEN_PRIVILEGES TokenPrivileges;
	BOOL bRet;
	HANDLE hToken;

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &TokenPrivileges.Privileges[0].Luid);   
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);

	TokenPrivileges.PrivilegeCount = 1;   
	TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	bRet = AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, NULL, NULL);

	CloseHandle(hToken);
	return bRet == TRUE;
}

DWORD GetProcessIdByName(char *in_szProcessName)
{
	DWORD dwProcessId = 0;

	HANDLE Snapshot;  
	Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	if( Snapshot == INVALID_HANDLE_VALUE )
	{
		return 0;
	}

	PROCESSENTRY32 ProcessListStr;   
	ProcessListStr.dwSize = sizeof(PROCESSENTRY32);   

	BOOL return_value;   
	return_value = Process32First(Snapshot, &ProcessListStr);   

	HANDLE hProcess;
	while(return_value)   
	{
		if (!stricmp(ProcessListStr.szExeFile, in_szProcessName))
		{   
			dwProcessId = ProcessListStr.th32ProcessID; 

			break;
		}

		return_value = Process32Next(Snapshot, &ProcessListStr);    
	}     

	CloseHandle(Snapshot);

	return dwProcessId;
}

double GetProcessCommitMemorySize(char *in_szProcessName)
{
	AdjustPurview();

	DWORD dwProcessId = GetProcessIdByName(in_szProcessName);
	if (dwProcessId == -1)
	{
		return 0;
	}

	//ȡ�ý��̵ľ��
	HANDLE hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, 
		dwProcessId );

	double dProcessMemorySize = 0;

	if (hProcess)
	{
		PROCESS_MEMORY_COUNTERS pmc; 
		GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)); 

		// �ύ
		/*
			PagefileUsage
			The Commit Charge value in bytes for this process. 
			Commit Charge is the total amount of memory that the memory manager 
			has committed for a running process.
		*/
		dProcessMemorySize = pmc.PagefileUsage * 1.0/(1024*1024);
	}

	CloseHandle(hProcess);

	return dProcessMemorySize;
}

PROCESS_MEMORY_COUNTERS GetProcessMemoryCounters(char *in_szProcessName)
{
	AdjustPurview();

	PROCESS_MEMORY_COUNTERS pmc; 
	memset(&pmc, 0, sizeof(PROCESS_MEMORY_COUNTERS));

	DWORD dwProcessId = GetProcessIdByName(in_szProcessName);
	if (dwProcessId == -1)
	{
		return pmc;
	}

	//ȡ�ý��̵ľ��
	HANDLE hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, 
		dwProcessId );

	double dProcessMemorySize = 0;

	if (hProcess)
	{
		GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)); 
	}

	CloseHandle(hProcess);

	return pmc;
}

int DeleteRunningProcess(LPCTSTR in_lpstrClientName)
{
	HANDLE Snapshot;  
	Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	if( Snapshot == INVALID_HANDLE_VALUE )
	{
		//g_logFile.WriteLogInfo("DeleteRunningProcess::CreateToolhelp32Snapshot Error");
		return -1;
	}

	PROCESSENTRY32 ProcessListStr;   
	ProcessListStr.dwSize = sizeof(PROCESSENTRY32);   

	BOOL return_value;   
	return_value = Process32First(Snapshot, &ProcessListStr);   

	HANDLE hProcess;
	while(return_value)   
	{
		if (!stricmp(ProcessListStr.szExeFile, in_lpstrClientName))
		{   
			hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, ProcessListStr.th32ProcessID );

			if (NULL != hProcess)
			{
				

				break;
			}
		}

		return_value = Process32Next(Snapshot,&ProcessListStr);    
	}     

	CloseHandle(Snapshot);

	return 0;
}


void PrintProcessNameAndID(DWORD processID)
{
	AdjustPurview();

	//ȡ�ý��̵ľ��
	HANDLE hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, 
		processID );

	char szProcessName[] = "filename";

	//ȡ�ý�������
	if ( hProcess )
	{
		//HMODULE hMod;
		//DWORD cbNeeded;
		//if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
		{
			GetModuleFileNameEx(hProcess, NULL, szProcessName,MAX_PATH);            
		}

		PROCESS_MEMORY_COUNTERS pmc; 

		GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)); 

		//���������������ʾΪ���ڴ棨ר�ù������� //���������������Ĭ����ʾ� (�����ڴ�)
		printf("%d/n", pmc.PagefileUsage/1024); 

		////���¸������KBΪ��λ 45.    
		////���������������ʾΪ����ֵ�������ã��ڴ棩 
		//printf("%d/n", pmc.PeakWorkingSetSize/1024); 
		////���������������ʾΪ���������ã��ڴ棩 
		//printf("%d/n", pmc.WorkingSetSize/1024); 
		//printf("%d/n", pmc.QuotaPeakPagedPoolUsage/1024); 
		////���������������ʾΪ����ҳ�� 
		//printf("%d/n", pmc.QuotaPagedPoolUsage/1024); 
		//printf("%d/n", pmc.QuotaPeakNonPagedPoolUsage/1024); 
		////���������������ʾΪ����ҳ�滺��� 
		//printf("%d/n", pmc.QuotaNonPagedPoolUsage/1024); 
		//printf("%d/n", pmc.PeakPagefileUsage/1024);
	}

	//���Խ������ƺ�ID
	printf( " %-20d%-20s", processID , szProcessName);

	CloseHandle( hProcess );
}


/*
//CreateProcess�ĵ�һ������    
STARTUPINFO StartupInfo = {0};    
StartupInfo.cb = sizeof(STARTUPINFO);  
//CreateProcess�ĵڶ�������    
PROCESS_INFORMATION ProcessInfo = {0};   

DWORD aProcesses[1024], cbNeeded, cProcesses;

unsigned int i;

//ö��ϵͳ����ID�б�

if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
{
	return 0;
}

//�����������

cProcesses = cbNeeded / sizeof(DWORD);

// ���ÿ�����̵����ƺ�ID

for ( i = 0; i < cProcesses; i++ )
{  
	PrintProcessNameAndID(aProcesses[i]);
}*/
