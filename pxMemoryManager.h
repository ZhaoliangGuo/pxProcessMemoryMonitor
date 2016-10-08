#include "stdafx.h"
#include <Psapi.h>
#include <iostream>
#include <string>
using namespace std;
#include "tlhelp32.h"

#pragma comment(lib, "psapi.lib") 

DWORD  GetProcessIdByName(char *in_szProcessName);
double GetProcessMemorySize(char *in_szProcessName);
