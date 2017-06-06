#ifndef LOSU_H
#define LOSU_H

#include <windows.h>

// low_functions.cpp
unsigned long MOD_GetProcAddress(
	HMODULE	modlue,
	char	*functionName
);
unsigned long ZwReadVirtualMemory(
	HANDLE	ProcessHandle,
	PVOID	BaseAddress,
	PVOID	Buffer,
	ULONG	BufferSize,
	PULONG	NumberOfBytesRead
);
unsigned long ZwWriteVirtualMemory(
	HANDLE	ProcessHandle,
	PVOID	BaseAddress,
	PVOID	Buffer,
	ULONG	BufferSize,
	PULONG	NumberOfBytesWritten
);



void *VZwOpenProcess(unsigned long pid);
void init_low_functions();

#endif
