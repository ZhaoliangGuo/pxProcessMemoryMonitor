#include "stdafx.h"
#include "pxMemoryManager.h"

//提升进程权限
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

	//取得进程的句柄
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

		// 提交
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

	//取得进程的句柄
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

	//取得进程的句柄
	HANDLE hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, 
		processID );

	char szProcessName[] = "filename";

	//取得进程名称
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

		//在任务管理器中显示为：内存（专用工作集） //这是任务管理器的默认显示项！ (虚拟内存)
		printf("%d/n", pmc.PagefileUsage/1024); 

		////以下各项均以KB为单位 45.    
		////在任务管理器中显示为：峰值工作设置（内存） 
		//printf("%d/n", pmc.PeakWorkingSetSize/1024); 
		////在任务管理器中显示为：工作设置（内存） 
		//printf("%d/n", pmc.WorkingSetSize/1024); 
		//printf("%d/n", pmc.QuotaPeakPagedPoolUsage/1024); 
		////在任务管理器中显示为：分页池 
		//printf("%d/n", pmc.QuotaPagedPoolUsage/1024); 
		//printf("%d/n", pmc.QuotaPeakNonPagedPoolUsage/1024); 
		////在任务管理器中显示为：非页面缓冲池 
		//printf("%d/n", pmc.QuotaNonPagedPoolUsage/1024); 
		//printf("%d/n", pmc.PeakPagefileUsage/1024);
	}

	//回显进程名称和ID
	printf( " %-20d%-20s", processID , szProcessName);

	CloseHandle( hProcess );
}


/*
//CreateProcess的第一个参数    
STARTUPINFO StartupInfo = {0};    
StartupInfo.cb = sizeof(STARTUPINFO);  
//CreateProcess的第二个参数    
PROCESS_INFORMATION ProcessInfo = {0};   

DWORD aProcesses[1024], cbNeeded, cProcesses;

unsigned int i;

//枚举系统进程ID列表

if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
{
	return 0;
}

//计算进程数量

cProcesses = cbNeeded / sizeof(DWORD);

// 输出每个进程的名称和ID

for ( i = 0; i < cProcesses; i++ )
{  
	PrintProcessNameAndID(aProcesses[i]);
}*/
