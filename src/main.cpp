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
    
    init_low_functions();
    hwnd = FindWindowA(0, windowName);
    
    if (hwnd) {
        pid = GetWindowThreadProcessId(hwnd, &pid);
        handle = VZwOpenProcess(pid);
        
        if (handle) {
            base = get_module(pid, "Neuz.exe");
            
            printf("base: %08X\n", base);
        } else printf("can't get control over %s\n", windowName);
    } else printf("can't find %s window\n", windowName);
    return 0;
}
