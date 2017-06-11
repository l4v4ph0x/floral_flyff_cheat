#include <windows.h>
#include <stdio.h>
#include "h/losu.h"
#include "h/summoner.h"
#include "h/gToolTip.h"
#include "res/resource.h"

void *handle;
unsigned long base;

unsigned long nr_range_addr;
bool bo_set_range = false;

HINSTANCE HIthis;
char windowName[] = "Floral Flyff";

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_SET_RANGE:
                    HWND hwnd;
                    char txt_range[256];
                    float nr_range;
                    
                    hwnd = GetDlgItem(hDlg, IDC_EDIT_RANGE);
                    GetWindowTextA(hwnd, txt_range, sizeof(txt_range) / sizeof(txt_range[0]));
                    
                    if (bo_set_range == false) {
                        // enabling range for everyone
                        ZwWriteVirtualMemory(handle, (void *)(base + 0x2A654A), (void *)"\x90\x90", 2, 0, true);
                        
                        // force to use set range
                        ZwWriteVirtualMemory(handle, (void *)(base + 0x2A6161), (void *)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 12, 0, true);
                        ZwWriteVirtualMemory(handle, (void *)(base + 0x2A616D + 2), &nr_range_addr, 4, 0, true);
                        
                        bo_set_range = true;
                    }
                    
                    nr_range = atof(txt_range);
                    sprintf(txt_range, "%f\n", nr_range);
                    printf(txt_range);
                    
                    // set range number
                    ZwWriteVirtualMemory(handle, (void *)(nr_range_addr), &nr_range, 4, 0);
                    SetWindowText(hwnd, txt_range);
                    
                    return true;
            }
            break;
        case WM_CHAR:
            printf("test");
            if ((wParam >= '0' && wParam <= '9') || wParam == '.' || wParam == VK_RETURN || wParam == VK_DELETE || wParam == VK_BACK)
                return 0;
            break;
        case WM_CLOSE:
            DestroyWindow(hDlg);
            return true;
        case WM_DESTROY:
            PostQuitMessage(0);
            return true;
    }
    
    return FALSE;
}

int main() {
    HWND hthis;
    HWND hwnd;
    unsigned int pid;
    
    init_low_functions();
    hthis = GetConsoleWindow();
    
    HIthis = (HINSTANCE)GetWindowLong(hthis, -6);
    
    hwnd = FindWindowA(0, windowName);
    if (hwnd) {
        GetWindowThreadProcessId(hwnd, &pid);
        handle = VZwOpenProcess(pid);
        
        if (handle) {
            // set variables
            base = 0x00400000; //get_module(pid, "Neuz.exe"); // get_module does not work on wine staging 2.9
            nr_range_addr = base + 0x005E56F8;
            
            HWND hDlg;
            hDlg = CreateDialogParam(HIthis, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
            ShowWindow(hDlg, SW_SHOW);
            
            // setting toolstips(balloon versions)
            gToolTip::AddTip(hDlg, HIthis, "Enter desired range number(in float). Ex: 100", IDC_EDIT_RANGE, true);
            
            BOOL ret;
            MSG msg;
            while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
                if (ret == -1) /* error found */
                    return -1;

                if (!IsDialogMessage(hDlg, &msg)) {
                    TranslateMessage(&msg); // translate virtual-key messages
                    DispatchMessage(&msg); // send it to dialog procedure
                }
            }
        } else {
            char msg[255];
            sprintf("Can't get control over %s\n", windowName);
            
            printf(msg);
            MessageBoxA(hthis, msg, "Error", 0);
        }
    } else {
        char msg[255];
        sprintf(msg, "Can't find %s window\n", windowName);
        
        printf(msg);
        MessageBoxA(hthis, msg, "Error", 0);
    }
    return 0;
}
