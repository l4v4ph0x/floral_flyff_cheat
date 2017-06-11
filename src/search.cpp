#include "h/summoner.h"
#include <tlhelp32.h>
#include <stdio.h>

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
	} return NULL;
}
