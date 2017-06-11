#include <windows.h>
#include <stdio.h>
#include "h/losu.h"
#include "h/summoner.h"
#include "h/gToolTip.h"
#include "h/flyff.h"
#include "res/resource.h"

// controller vars
flyff f;

// flyff and this window info
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
                    
                    // get control
                    hwnd = GetDlgItem(hDlg, IDC_EDIT_RANGE);
                    // get control text, in this case edittext
                    GetWindowTextA(hwnd, txt_range, sizeof(txt_range) / sizeof(txt_range[0]));
                    
                    // convert it to float and then print it out
                    nr_range = atof(txt_range);
                    sprintf(txt_range, "%f\n", nr_range);
                    printf(txt_range);
                    
                    // set range and change edittext value of converted val
                    f.set_range(nr_range);
                    SetWindowText(hwnd, txt_range);
                    
                    return true;
                case ID_SELECT_TARGET:
                    flyff::targetInfo ti;
                    
                    ti = f.getClosestTargetInView();
                    printf("%08X\n", ti.base);
                    if (ti.base != 0) {
                        f.select(ti.base);
                    }
            }
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
    void *handle;
    unsigned int pid;
    unsigned long base;
    unsigned long base_size;
    HWND hthis;
    HWND hwnd;
    
    HWND hDlg;
    BOOL ret;
    MSG msg;
    
    char buf_msg[64];
    
    init_low_functions();
    
    // get current window vars
    hthis = GetConsoleWindow();
    HIthis = (HINSTANCE)GetWindowLong(hthis, -6);
    
    hwnd = FindWindowA(0, windowName);
    if (hwnd) {
        // get privs to mod proc
        GetWindowThreadProcessId(hwnd, &pid);
        handle = VZwOpenProcess(pid);
        
        if (handle) {
            // set variables
            base = 0x00400000; //get_module(pid, "Neuz.exe", &base_size); // get_module does not work on wine staging 2.9
            base_size = 0x00917000; // this and base from immunity debugger
            
            // loading flyff class
            f = flyff(handle, base, base_size);
            
            // create window
            hDlg = CreateDialogParam(HIthis, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
            ShowWindow(hDlg, SW_SHOW);
            
            // setting toolstips(balloon versions)
            gToolTip::AddTip(hDlg, HIthis, "Enter desired range number(in float). Ex: 100", IDC_EDIT_RANGE, true);
            
            // listen window inputs
            while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
                if (ret == -1) // error found
                    return -1;

                if (!IsDialogMessage(hDlg, &msg)) {
                    TranslateMessage(&msg); // translate virtual-key messages
                    DispatchMessage(&msg); // send it to dialog procedure
                }
            }
        } else {
            sprintf(buf_msg, "Can't get control over %s\n", windowName);
            
            printf(buf_msg);
            MessageBoxA(hthis, buf_msg, "Error", 0);
        }
    } else {
        sprintf(buf_msg, "Can't find %s window\n", windowName);
        
        printf(buf_msg);
        MessageBoxA(hthis, buf_msg, "Error", 0);
    }
    return 0;
}
