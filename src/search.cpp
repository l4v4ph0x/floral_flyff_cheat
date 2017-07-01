#include "h/summoner.h"

#include <tlhelp32.h>
#include <stdio.h>
#include <vector>

#define size_to_scan 786

unsigned long get_module(unsigned long pid, char *module_name, unsigned long *size) {
	void *snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
    
    printf("go while\n");
	while (Module32Next(snapshot, &me32)) {
        printf("\tmodule: %s, %08X\n", me32.szModule, me32.modBaseAddr);
		if (strcmp(me32.szModule, module_name) == 0) {
			if (size != 0) *size = me32.modBaseSize;
				return (unsigned long)me32.modBaseAddr;
		}
	} return 0;
}

std::vector<unsigned long> get_procs(char *name) {
    std::vector<unsigned long> pids;
    
	void *snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
    
	while (Process32Next(snapshot, &pe32))
		if (strcmp(pe32.szExeFile, name) == 0)
            pids.push_back(pe32.th32ProcessID);
	
	return pids;
}

unsigned long search(void *handle, unsigned long start, unsigned long size, const char *bytesToSearch, unsigned long sizeToSearch, unsigned long step) {
	unsigned char bytes[size_to_scan];
	unsigned long gotCount = 0;

	for (unsigned long i = 0; i < size; i += size_to_scan) {
		ReadProcessMemory(handle, (PVOID)(start + i), &bytes, size_to_scan, 0);
		for (unsigned short j = 0; j < size_to_scan; j++) {
			bool failed = false;

			for (unsigned char k = 0; k < sizeToSearch; k++) {
				if (*(unsigned char *)((unsigned long)&bytes + j + k) != (unsigned char)bytesToSearch[k]) {
					failed = true;
					break;
				}
			}

			if (failed == false) {
				gotCount++;
				if (gotCount == step)
					return start + i + j;
			}
		}
	} return 0;
}

struct handle_data {
    unsigned long process_id;
    HWND best_handle;
};

BOOL is_main_window(HWND handle)
{   
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
    handle_data& data = *(handle_data*)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, (LPDWORD)&process_id);
    if (data.process_id != process_id || !is_main_window(handle)) {
        return TRUE;
    }
    data.best_handle = handle;
    return FALSE;   
}


HWND find_main_window(unsigned long process_id)
{
    handle_data data;
    data.process_id = process_id;
    data.best_handle = 0;
    EnumWindows(enum_windows_callback, (LPARAM)&data);
    return data.best_handle;
}


