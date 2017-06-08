#include <windows.h>
#include <stdio.h>
#include "losu.h"
#include "summoner.h"


char windowName[] = "Floral Flyff";

int main() {
    HWND hwnd;
    unsigned long pid;
    void *handle;
    unsigned long base;
    unsigned long oldProtect;
    
    unsigned long nr_range_addr;
    
    float nr_range;
    
    init_low_functions();
    hwnd = FindWindowA(0, windowName);
    
    if (hwnd) {
        GetWindowThreadProcessId(hwnd, &pid);
        handle = VZwOpenProcess(pid);
        
        if (handle) {
            // set variables
            base = 0x00400000; //get_module(pid, "Neuz.exe"); // get_module does not work on wine staging 2.9
            nr_range_addr = base + 0x005E56F8;
            
            // fill variables
            nr_range = 100.f;
            
            // set range number
            ZwWriteVirtualMemory(handle, (void *)(nr_range_addr), &nr_range, 4, 0);
            
            // enabling range for everyone
            ZwWriteVirtualMemory(handle, (void *)(base + 0x2A654A), (void *)"\x90\x90", 2, 0, true);
            
            // force to use set range
            ZwWriteVirtualMemory(handle, (void *)(base + 0x2A6161), (void *)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 12, 0, true);
            ZwWriteVirtualMemory(handle, (void *)(base + 0x2A616D + 2), &nr_range_addr, 4, 0, true);
        } else printf("can't get control over %s\n", windowName);
    } else printf("can't find %s window\n", windowName);
    return 0;
}
