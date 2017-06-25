#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <vector>

#include "h/losu.h"
#include "h/summoner.h"
#include "h/gToolTip.h"
#include "h/flyff.h"
#include "res/resource.h"
#include "res/values.h"

// controller vars
flyff f;

// select thread vars
void *h_select_thread;
unsigned long ul_key;

// flyff and this window info
HINSTANCE HIthis;
char windowName[] = "Floral Flyff";

HWND hTabControl; // tab control handle
HWND hCurrentTab; // tab dialog handle

unsigned long __stdcall _thread_select_target(void *t) {
    //flyff _f = *((flyff *)t); // main class for every client
   
    flyff::targetInfo ti;
    f.select(0);
    
    for (;; Sleep(100)) {
        if (f.getSelect() == 0) {
            // rotate cam
            PostMessage((HWND)f.get_hwnd(), WM_KEYDOWN, VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC));
            Sleep(20);
            PostMessage((HWND)f.get_hwnd(), WM_KEYUP, VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC));
            
            ti = f.getClosestTargetInView();
            
            if (ti.base != 0) {
                printf("closest target: %08X\n", ti.base);
                if (ti.base != 0) {
                    f.select(ti.base);
                }   
            }
        } else {
            Sleep(20);
            PostMessage((HWND)f.get_hwnd(), WM_KEYDOWN, ul_key, MapVirtualKey(ul_key, MAPVK_VK_TO_VSC));
            Sleep(20);
            PostMessage((HWND)f.get_hwnd(), WM_KEYUP, ul_key, MapVirtualKey(ul_key, MAPVK_VK_TO_VSC));
        }
    }
    
    return 0;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HWND hwnd;
    char txt_buf[256];
    
    switch (uMsg) {
        case WM_INITDIALOG:
            int begin, end, i;
            
            // set target selector button to enable
            //hwnd = GetDlgItem(hDlg, ID_TARGET_ENABLE);
            //SetWindowText(hwnd, STR_ENABLE_TARGET);
            
            // set target levels range
            //f.get_target_lvls(&begin, &end);
                // set begin one
           //     sprintf(txt_buf, "%d", begin);
           //     hwnd = GetDlgItem(hDlg, IDC_EDIT_TARGET_LVL_BEGIN);
           //     SetWindowText(hwnd, txt_buf);
                // set end one
           //     sprintf(txt_buf, "%d", end);
           //     hwnd = GetDlgItem(hDlg, IDC_EDIT_TARGET_LVL_END);
           //     SetWindowText(hwnd, txt_buf);
            
            // adding items to combobox
            //hwnd = GetDlgItem(hDlg, IDC_COMBO_KEYS);
            
            //for (i = 0; i < sizeof(combo_items) / sizeof(comboItem); i++)
            //    SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)combo_items[i].name);
            //SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
            //ul_key = combo_items[0].val;
            
            //hwnd = GetDlgItem(hDlg, IDC_EDIT_BTC_ADDR);
            //SetWindowText(hwnd, STR_BTC_ADDR);
            
            
            // setting tabs
            INITCOMMONCONTROLSEX ix;
            ix.dwSize = sizeof(INITCOMMONCONTROLSEX);
            ix.dwICC = ICC_TAB_CLASSES;
            InitCommonControlsEx(&ix);
            hTabControl = GetDlgItem(hDlg, IDC_TAB_MAIN);
            
            TCITEM ti;
            ti.mask = TCIF_TEXT;
            ti.pszText = "Select Flyff";
            TabCtrl_InsertItem(hTabControl, 0, &ti);
            TabCtrl_SetCurSel(hTabControl, 0);
            hCurrentTab = CreateDialog(HIthis, MAKEINTRESOURCE(IDD_TAB_SELECT_FLYFF), hTabControl, 0);
            
            std::vector<comboItem> combo_names;
            std::vector<unsigned long> pids = get_procs("Neuz.exe");
            
            for (i = 0; i < pids.size(); i++) {
                comboItem ci;
                ci.name = "fd";
                combo_names.push_back(ci);
            }

            // adding items to combobox
            hwnd = GetDlgItem(hCurrentTab, IDC_COMBO_PLAYERS);
            
            for (i = 0; i < combo_names.size(); i++)
                SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)combo_names[i].name);
            SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
            
            printf("done\n");
            
            return true;
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code) {
                case TCN_SELCHANGING:
                    EndDialog(hCurrentTab, 0);
                    
                    if (TabCtrl_GetCurSel(hTabControl) == 0)
                        hCurrentTab = CreateDialog(HIthis, MAKEINTRESOURCE(IDD_TAB_SELECT_FLYFF), hTabControl, 0);
                    
                    return true;
                
                break;
            }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_SET_RANGE:
                    float nr_range;
                    
                    // get control
                    hwnd = GetDlgItem(hDlg, IDC_EDIT_RANGE);
                    // get control text, in this case edittext
                    GetWindowTextA(hwnd, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
                    
                    // convert it to float and then print it out
                    nr_range = atof(txt_buf);
                    sprintf(txt_buf, "%f\n", nr_range);
                    printf(txt_buf);
                    
                    // set range and change edittext value of converted val
                    f.set_range(nr_range);
                    SetWindowText(hwnd, txt_buf);
                    
                    return true;
                case ID_TARGET_ENABLE:
                    // get control
                    hwnd = GetDlgItem(hDlg, ID_TARGET_ENABLE);
                    
                    if (h_select_thread == nullptr) {
                        h_select_thread = CreateThread(0, 0, _thread_select_target, 0, 0, 0);
                        
                        // set window text to disable target selector
                        SetWindowText(hwnd, STR_DISABLE_TARGET);
                    } else {
                        TerminateThread(h_select_thread, 0);
                        h_select_thread = nullptr;
                        
                        // set windw text to enable target selector
                        SetWindowText(hwnd, STR_ENABLE_TARGET);
                    }
                    
                    return true;
                case IDC_EDIT_TARGET_LVL_BEGIN:
                    switch (HIWORD(wParam)) {
                        case EN_CHANGE:
                            int nr_lvl;
                            
                            GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
                            nr_lvl = atoi(txt_buf);
                            
                            f.set_target_lvls(nr_lvl);
                            
                            return true;
                        case EN_KILLFOCUS:
                            int begin, end;
                            
                            f.get_target_lvls(&begin, &end);
                            
                            sprintf(txt_buf, "%d", begin);
                            SetWindowText((HWND)lParam, txt_buf);
                            
                            return true;
                    }
                    
                    break;
                case IDC_EDIT_TARGET_LVL_END:
                    switch (HIWORD(wParam)) {
                        case EN_CHANGE:
                            int nr_lvl;
                            
                            GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
                            nr_lvl = atoi(txt_buf);
                            
                            f.set_target_lvls(-1, nr_lvl);
                            
                            return true;
                        case EN_KILLFOCUS:
                            int begin, end;
                            
                            f.get_target_lvls(&begin, &end);
                            
                            sprintf(txt_buf, "%d", end);
                            SetWindowText((HWND)lParam, txt_buf);
                            
                            return true;
                    }
                    
                    break;
                case IDC_COMBO_KEYS:
                    switch (HIWORD(wParam)) {
                        case CBN_SELCHANGE:
                            int i;
                            
                            i = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                            
                            
                            printf("%s %x\n", combo_items[i].name, combo_items[i].val);
                            ul_key = combo_items[i].val;
                                   
                            
                            return true;
                    }
                    
                    break;
            }
            
            break;
        case WM_CLOSE:
            DestroyWindow(hDlg);
            return true;
        case WM_DESTROY:
            PostQuitMessage(0);
            return true;
    }
    
    return false;
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
    
    // create window
    hDlg = CreateDialogParam(HIthis, MAKEINTRESOURCE(IDD_DIALOG_MAIN), 0, DialogProc, 0);
    ShowWindow(hDlg, SW_SHOW);
    
    // listen window inputs
    while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
        if (ret == -1) // error found
            return -1;

        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg); // translate virtual-key messages
            DispatchMessage(&msg); // send it to dialog procedure
        }
    }
    /*
    hwnd = FindWindowA(0, windowName);
    if (hwnd) {
        // get privs to mod proc
        GetWindowThreadProcessId(hwnd, (LPDWORD)&pid);
        handle = VZwOpenProcess(pid);
        
        if (handle) {
            // get base and size if available
			base = get_module(pid, "Neuz.exe", &base_size);

			if (base == 0) {
				// set variables
				base = 0x00400000; //get_module(pid, "Neuz.exe", &base_size); // get_module does not work on wine staging 2.9
				base_size = 0x00917000; // this and base from immunity debugger
			}
            
            // loading flyff class
            f = flyff(handle, base, base_size);
            // set default target levels range
            f.set_target_lvls(1, 400);
            // set flyff window handle
            f.set_hwnd((void *)hwnd);
            
            // null select thread
            h_select_thread = nullptr;
            
            
            
            
            // setting toolstips(balloon versions)
            //gToolTip::AddTip(hDlg, HIthis, "Enter desired range number(in float). Ex: 100", IDC_EDIT_RANGE, true);
            //gToolTip::AddTip(hDlg, HIthis, "Enter desired lowest level to select. Ex: 1", IDC_EDIT_TARGET_LVL_BEGIN, true);
            //gToolTip::AddTip(hDlg, HIthis, "Enter desired highest level to select. Ex: 22", IDC_EDIT_TARGET_LVL_END, true);
            
            
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
    
    */
    return 0;
}
