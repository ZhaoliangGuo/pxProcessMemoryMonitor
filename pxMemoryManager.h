#include "stdafx.h"
#include <Psapi.h>
#include <iostream>
#include <string>
using namespace std;
#include "tlhelp32.h"

#pragma comment(lib, "psapi.lib") 

DWORD  GetProcessIdByName(char *in_szProcessName);
double GetProcessCommitMemorySize(char *in_szProcessName);
PROCESS_MEMORY_COUNTERS GetProcessMemoryCounters(char *in_szProcessName);
