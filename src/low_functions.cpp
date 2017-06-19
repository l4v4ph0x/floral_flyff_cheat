#include "h/losu.h"
#include "h/lowlvl.h"

// {{{ ----- low lvl opening
    // {{{ ----- typedefs
	typedef long(__stdcall *TMOD_GetProcAddress)(
		HMODULE	modlue,
		char	*functionName
		); TMOD_GetProcAddress pMOD_GetProcAddress = NULL;
    typedef long(__stdcall *TZwReadVirtualMemory)(
		HANDLE	ProcessHandle,
		PVOID	BaseAddress,
		PVOID	Buffer,
		ULONG	BufferSize,
		PULONG	NumberOfBytesRead
		); TZwReadVirtualMemory pZwReadVirtualMemory = NULL;
    typedef long(__stdcall *TZwWriteVirtualMemory)(
		HANDLE	ProcessHandle,
		PVOID	BaseAddress,
		PVOID	Buffer,
		ULONG	BufferSize,
		PULONG	NumberOfBytesWritten
		); TZwWriteVirtualMemory pZwWriteVirtualMemory = NULL;
    typedef long(__stdcall *TZwOpenProcess)(
		PHANDLE				ProcessHandle,
		ACCESS_MASK			DesiredAccess,
		POBJECT_ATTRIBUTES	ObjectAttributes,
		PCLIENT_ID			ClientId
		); TZwOpenProcess pZwOpenProcess = NULL;
	// ----- }}}


unsigned long MOD_GetProcAddress(
	HMODULE	modlue,
	char	*functionName
	) {
	return pMOD_GetProcAddress(modlue, functionName);
}
unsigned long ZwReadVirtualMemory(
	HANDLE	ProcessHandle,
	PVOID	BaseAddress,
	PVOID	Buffer,
	ULONG	BufferSize,
	PULONG	NumberOfBytesRead
	) {
	return pZwReadVirtualMemory(ProcessHandle, BaseAddress, Buffer, BufferSize, NumberOfBytesRead);
}

unsigned long ZwWriteVirtualMemory(
	HANDLE	ProcessHandle,
	PVOID	BaseAddress,
	PVOID	Buffer,
	ULONG	BufferSize,
	PULONG	NumberOfBytesWritten,
	bool safe
	) {
    
    unsigned int oldProtect;
    
    if (safe == true) VirtualProtectEx(ProcessHandle, BaseAddress, BufferSize, PAGE_EXECUTE_READWRITE, &oldProtect);
        unsigned long ret = pZwWriteVirtualMemory(ProcessHandle, BaseAddress, Buffer, BufferSize, NumberOfBytesWritten);
    if (safe == true) VirtualProtectEx(ProcessHandle, BaseAddress, BufferSize, oldProtect, NULL);
    
    return ret;
}
unsigned long ZwOpenProcess(
	PHANDLE				ProcessHandle,
	ACCESS_MASK			DesiredAccess,
	POBJECT_ATTRIBUTES	ObjectAttributes,
	PCLIENT_ID			ClientId
	) {
	return pZwOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}

void *VZwOpenProcess(unsigned long pid) {
	void *handle;

	OBJECT_ATTRIBUTES objAttr;
	CLIENT_ID cID;

	InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

	cID.UniqueProcess = (PVOID)pid;
	cID.UniqueThread = 0;

	ZwOpenProcess(&handle, PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME, &objAttr, &cID);

	return handle;
}
// ----- }}}

void init_low_functions() {
	HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    
   	pMOD_GetProcAddress			= (TMOD_GetProcAddress)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "GetProcAddress");
	pZwOpenProcess              = (TZwOpenProcess)MOD_GetProcAddress(ntdll, "ZwOpenProcess");
    pZwReadVirtualMemory        = (TZwReadVirtualMemory)MOD_GetProcAddress(ntdll, "ZwReadVirtualMemory");
    pZwWriteVirtualMemory       = (TZwWriteVirtualMemory)MOD_GetProcAddress(ntdll, "ZwWriteVirtualMemory");
}
