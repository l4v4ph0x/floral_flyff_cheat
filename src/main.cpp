#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <vector>

#include "a/flyff.h"

#include "flyffs/floral_flyff.h"
#include "flyffs/shining_nation.h"

#include "h/losu.h"
#include "h/summoner.h"
#include "h/gToolTip.h"
#include "h/texts.h"
#include "res/resource.h"
#include "res/values.h"

// controller vars
std::vector<flyff *> fs;

unsigned int when_close_noti;

// flyff and this window info
HINSTANCE HIthis;
char windowName[] = "Floral Flyff";

HWND hTabControl; // tab control handle
HWND hCurrentTab; // tab dialog handle
int tabCount;
flyff *fCurrentTab;

INT_PTR CALLBACK TabDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

void add_tab(char *title, unsigned int tab, LPARAM lParam = 0) {
	BOOL ret;
	MSG msg;

	// add tab
	TCITEM ti;
	ti.mask = TCIF_TEXT;
	ti.pszText = title;
    ti.lParam = lParam;
	TabCtrl_InsertItem(hTabControl, tabCount, &ti);
	TabCtrl_SetCurSel(hTabControl, tabCount++);

	EndDialog(hCurrentTab, 0);
	hCurrentTab = CreateDialog(HIthis, MAKEINTRESOURCE(tab), hTabControl, TabDialogProc);
}

void close_tab(unsigned int index) {
	// kill tab
	TabCtrl_DeleteItem(hTabControl, index + 1);
	free(fs[index]);
	fs.erase(fs.begin() + index - 1);

	if (index + 2 >= tabCount)
		SendMessage(hTabControl, TCM_SETCURFOCUS, index, 0);
	else SendMessage(hTabControl, TCM_SETCURFOCUS, index + 1, 0);
}

unsigned long __stdcall _thread_hide_noti(void *t) {
    HWND hwnd;
    unsigned int miliseconds;
    
    miliseconds = 0;
    hwnd = GetDlgItem(hCurrentTab, ID_NOTI);
    
    // sleep miliseconds that noti should show and sleep more if new noti has be created
    for (; when_close_noti > 0; Sleep(miliseconds)) {
        miliseconds = when_close_noti - miliseconds;
        when_close_noti = 0;
    }
    
    ShowWindow(hwnd, SW_HIDE);
    
    return 0;
}

unsigned long __stdcall _thread_if_connected(void *t) {
	unsigned int index = *(unsigned int *)t -2;

	for (;; Sleep(1000)) {
		//printf("%d %08X\n", index, fs[index].getMe());

		if (!fs[index]->localPlayer->get_me())
			break;
	}

	close_tab(index);
	
	return 0;
}

void show_noti(char *txt, unsigned int miliseconds) {
    HWND hwnd;
    
    when_close_noti = miliseconds;
    hwnd = GetDlgItem(hCurrentTab, ID_NOTI);
    
    SetWindowText(hwnd, txt);
    ShowWindow(hwnd, SW_SHOW);
    CreateThread(0, 0, _thread_hide_noti, 0, 0, 0);
}

flyff *get_flyff_by_pid(unsigned long pid) {
    char txt_buf[256];

    flyff *f = new floral_flyff(pid);

    if (f->error_string == nullptr) {
        return f;
    } else {
        printf("error: %s\n\n", f->error_string);
        // delete class cause we open it again when selected
        free(f);

        f = new shining_nation(pid);

        if (f->error_string == nullptr) {
            return f;
        }
        else printf("error: %s\n\n", f->error_string);
    }

    return nullptr;
}

INT_PTR CALLBACK TabDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND hwnd;
	char txt_buf[256];
    char newbuf[256];
    std::vector<unsigned long> pids;
    int index, i;
    flyff *f;
    
	switch (uMsg) {
		case WM_INITDIALOG: {
            // get index of selected tab
			index = TabCtrl_GetCurFocus(hTabControl);

			// if tab is select flyff
			if (index == 0) {
                // get processes
                pids = get_procs("Neuz.exe");

				for (i = 0; i < pids.size(); i++) {
                    f = get_flyff_by_pid(pids[i]);

                    if (f != nullptr) {
                        f->localPlayer->get_name(txt_buf);
                        SendMessage(GetDlgItem(hDlg, IDC_COMBO_PLAYERS), CB_ADDSTRING, 0, (LPARAM)txt_buf);
                    }
                    // delete class cause we open it again when selected
					free(f);
				}

                // select first character if any
                if (pids.size() > 0)
                    SendMessage(GetDlgItem(hDlg, IDC_COMBO_PLAYERS), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			} else {
                int begin, end;
                double d;
                bool use;
				flyff::key k;

                fCurrentTab = fs[index -1];
                
                fCurrentTab->localPlayer->get_name(txt_buf);
                printf("loading tab: %s\n", txt_buf);
                
				// disabling perin converter(need update, need to search hdd and get patterns)
				EnableWindow(GetDlgItem(hDlg, IDC_CHECBKOX_PERIN_CONVERTER), false);

                // set bot status hwnd
                fCurrentTab->ui->set_hwnd_noti(GetDlgItem(hDlg, IDC_STATIC_BOT_STATUS));
                
                // setting no collision ceckbox
                hwnd = GetDlgItem(hDlg, IDC_CHECBKOX_NO_COLLISION);
                use = fCurrentTab->localPlayer->get_no_collision();
                if (use)
                    SendMessage(hwnd, BM_SETCHECK, BST_CHECKED, 0);
                else
                    SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
                
                // hide noti text
                hwnd = GetDlgItem(hDlg, ID_NOTI);
                ShowWindow(hwnd, SW_HIDE);
                
                // load tele to target and back home
                d = fCurrentTab->bot->get_kill_to_home();
                if (d == 0) {
					// unchecked at start anyway
                    // uncheck checkbox and edittext
                    //hwnd = GetDlgItem(hDlg, IDC_CHECBKOX_TELE_TARGET_HOME);
                    //SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
                    //hwnd = GetDlgItem(hDlg, IDC_EDIT_TELE_HOME_AFTER_KILLS);
                    //EnableWindow(hwnd, false);
                } else {
                    // check checkbox and edittext
                    hwnd = GetDlgItem(hDlg, IDC_CHECBKOX_TELE_TARGET_HOME);
                    SendMessage(hwnd, BM_SETCHECK, BST_CHECKED, 0);
                    hwnd = GetDlgItem(hDlg, IDC_EDIT_TELE_HOME_AFTER_KILLS);
                    sprintf(txt_buf, "%.0f", fCurrentTab->bot->get_kill_to_home());
                    SetWindowText(hwnd, txt_buf);
                    EnableWindow(hwnd, true);
                }
                
                // set target selector button to enable
                hwnd = GetDlgItem(hDlg, ID_TARGET_ENABLE);
                SetWindowText(hwnd, STR_ENABLE_TARGET);

                // set target levels range
                fCurrentTab->bot->get_target_lvls(&begin, &end);
                    // set begin one
                    sprintf(txt_buf, "%d", begin);
                    hwnd = GetDlgItem(hDlg, IDC_EDIT_TARGET_LVL_BEGIN);
                    SetWindowText(hwnd, txt_buf);
                    // set end one
                    sprintf(txt_buf, "%d", end);
                    hwnd = GetDlgItem(hDlg, IDC_EDIT_TARGET_LVL_END);
                    SetWindowText(hwnd, txt_buf);
                    
                // adding items to combobox
                hwnd = GetDlgItem(hDlg, IDC_COMBO_KEYS);

                for (i = 0; i < sizeof(combo_items) / sizeof(comboItem); i++)
                    SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)combo_items[i].name);
                
                if (!fCurrentTab->bot->get_key(&k)) {
                    SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
                    fCurrentTab->bot->add_update_attack_key(flyff::key(combo_items[0].val, 100.f));
                } else {
					printf("val: %08X\n", k.code);
                    for (i = 0; i < sizeof(combo_items) / sizeof(comboItem); i++) {
                        if (combo_items[i].val == k.code) {
                            SendMessage(hwnd, CB_SETCURSEL, (WPARAM)i, (LPARAM)0);
                            break;
                        }
                    }
                    
                }

				// load perin converter checkbox
				hwnd = GetDlgItem(hDlg, IDC_CHECBKOX_PERIN_CONVERTER);
                use = fCurrentTab->get_perin_convert_spam();
                if (use)
                    SendMessage(hwnd, BM_SETCHECK, BST_CHECKED, 0);
                else
                    SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0);

				// load reselect target after seconds
				hwnd = GetDlgItem(hDlg, IDC_EDIT_RESELECT_AFTER);
				sprintf(txt_buf, "%d", fCurrentTab->bot->get_reselect_after());
				SetWindowText(hwnd, txt_buf);
                
				// { --- adding items to hper combobox
				hwnd = GetDlgItem(hDlg, IDC_COMBO_HP_KEYS);

				for (i = 0; i < sizeof(combo_items) / sizeof(comboItem); i++)
					SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)combo_items[i].name);

				if (!fCurrentTab->buff->get_hp_key(&k)) {
					SendMessage(hwnd, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					fCurrentTab->buff->set_hp_key(flyff::key(combo_items[1].val, 100.f));
				} else {
					printf("val: %08X\n", k.code);
					for (i = 0; i < sizeof(combo_items) / sizeof(comboItem); i++) {
						if (combo_items[i].val == k.code) {
							SendMessage(hwnd, CB_SETCURSEL, (WPARAM)i, (LPARAM)0);
							break;
						}
					}
				}
				// --- }

				// set tp heal hp text
				hwnd = GetDlgItem(hDlg, IDC_EDIT_HP);
				sprintf(txt_buf, "%d", fCurrentTab->buff->get_hp_to_buff());
				SetWindowText(hwnd, txt_buf);

                
                // setting toolstips(balloon versions)
                //gToolTip::AddTip(hCurrentTab, HIthis, "Enter desired range number(in float). Ex: 100", IDC_EDIT_RANGE, true);
                //gToolTip::AddTip(hTabControl, HIthis, "Enter desired lowest level to select. Ex: 1", IDC_EDIT_TARGET_LVL_BEGIN, true);
                //gToolTip::AddTip(hDlg, HIthis, "Enter desired highest level to select. Ex: 22", IDC_EDIT_TARGET_LVL_END, true);
            }
            
			return true;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case ID_HOOK: {
					hwnd = GetDlgItem(hCurrentTab, IDC_COMBO_PLAYERS);
					GetWindowTextA(hwnd, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
					printf("hook to player: %s\n", txt_buf);
                    
                    pids = get_procs("Neuz.exe");
                    
                    // adding new flyff class
                    for (i = 0; i < pids.size(); i++) {
                        f = get_flyff_by_pid(pids[i]);
                        f->localPlayer->get_name(newbuf);
                        
                        if (strcmp(txt_buf, newbuf) == 0) {
                            // set default target levels range
                            f->bot->set_target_lvls(1, 400);
                            // set flyff window handle
                            f->ui->set_hwnd((void *)find_main_window(pids[i]));

                            fs.push_back(f);
                            printf("Noice!\n");
                            add_tab(txt_buf, IDD_TAB_FLYFF, (LPARAM)&f);

							CreateThread(0, 0, _thread_if_connected, &tabCount, 0, 0);

                            break;
                        }
					} break;
				}
                
				case ID_SET_RANGE: {
					float nr_range;

					// get control
					hwnd = GetDlgItem(hDlg, IDC_EDIT_RANGE);
					// get control text, in this case edittext
					GetWindowTextA(hwnd, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));

					// convert it to float and then print it out
					nr_range = atof(txt_buf);
                    printf("setting range to: %f\n", nr_range);

					// set range and change edittext value of converted val
                    nr_range = fCurrentTab->localPlayer->set_range(nr_range);

                    sprintf(txt_buf, "%f", nr_range);
					SetWindowText(hwnd, txt_buf);

					break;
                }
				case IDC_EDIT_TARGET_LVL_BEGIN: {
					switch (HIWORD(wParam)) {
						case EN_CHANGE: {
							int begin, end, nr_lvl;

							GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
							nr_lvl = atoi(txt_buf);
							fCurrentTab->bot->get_target_lvls(&begin, &end);
                            
                            // if really changed, then only change in class too
                            // and is it bigger then 0
                            if (nr_lvl != begin && nr_lvl > 0) {
                                fCurrentTab->bot->set_target_lvls(nr_lvl);
                                
                                if (fCurrentTab->bot->get_run())
                                    show_noti((char *)texts::noti_reenable_bot, 6000);
							} break;
                        }
						case EN_KILLFOCUS: {
							int begin, end;

							fCurrentTab->bot->get_target_lvls(&begin, &end);
							sprintf(txt_buf, "%d", begin);
							SetWindowText((HWND)lParam, txt_buf);

							break;
                        } break;
					} break;
                }
				case IDC_EDIT_TARGET_LVL_END: {
					switch (HIWORD(wParam)) {
						case EN_CHANGE: {
							int begin, end, nr_lvl;

							GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
							nr_lvl = atoi(txt_buf);
                            fCurrentTab->bot->get_target_lvls(&begin, &end);
                            
                            // if really changed, then only change in class too
                            if (nr_lvl != end) {
                                fCurrentTab->bot->set_target_lvls(-1, nr_lvl);
                                
                                if (fCurrentTab->bot->get_run())
                                    show_noti((char *)texts::noti_reenable_bot, 6000);
							} break;
                        }
						case EN_KILLFOCUS: {
							int begin, end;

							fCurrentTab->bot->get_target_lvls(&begin, &end);
							sprintf(txt_buf, "%d", end);
							SetWindowText((HWND)lParam, txt_buf);

							break;
                        } break;
					} break;
                }
                case IDC_COMBO_KEYS: {
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						i = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						printf("selected f key: %s %x\n", combo_items[i].name, combo_items[i].val);
                        fCurrentTab->bot->add_update_attack_key(flyff::key(combo_items[i].val, 100.f));
                        
                        if (fCurrentTab->bot->get_run())
                            show_noti((char *)texts::noti_reenable_bot, 6000);
					} break;
                }
                case ID_TARGET_ENABLE: {
					// get control
					hwnd = GetDlgItem(hDlg, ID_TARGET_ENABLE);

					if (fCurrentTab->bot->get_run() == false) {
                        if (fCurrentTab->bot->run()) {
						    // set window text to disable target selector
						    SetWindowText(hwnd, STR_DISABLE_TARGET);
                        }
					} else {
						fCurrentTab->bot->stop();

						// set windw text to enable target selector
						SetWindowText(hwnd, STR_ENABLE_TARGET);
					} break;
                }
                case IDC_CHECBKOX_TELE_TARGET_HOME: {
                    if (HIWORD(wParam) == BN_CLICKED) {
                        bool checked = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
                        hwnd = GetDlgItem(hDlg, IDC_EDIT_TELE_HOME_AFTER_KILLS);
                        
                        if (checked == true) {
                            EnableWindow(hwnd, true);
                            fCurrentTab->bot->set_kill_to_home(3);
                            sprintf(txt_buf, "%.0f", fCurrentTab->bot->get_kill_to_home());
                            SetWindowText(hwnd, txt_buf);
                            SetFocus(hwnd);
                        } else {
                            EnableWindow(hwnd, false);
                            fCurrentTab->bot->set_kill_to_home(0);
                            SetWindowText(hwnd, "");
                        }
                        
                        if (fCurrentTab->bot->get_run())
                            show_noti((char *)texts::noti_reenable_bot, 6000);
					} break;
                }
                case IDC_EDIT_TELE_HOME_AFTER_KILLS: {
                    switch (HIWORD(wParam)) {
						case EN_CHANGE: {
							double fl;

                            hwnd = GetDlgItem(hDlg, IDC_CHECBKOX_TELE_TARGET_HOME);
							GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
							fl = atof(txt_buf);
							
                            if (fl != fCurrentTab->bot->get_kill_to_home()) {
                                if (fl <= 0) {
                                    fCurrentTab->bot->set_kill_to_home(0);
                                    SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
                                } else {
                                    fCurrentTab->bot->set_kill_to_home(fl);
                                    SendMessage(hwnd, BM_SETCHECK, BST_CHECKED, 0);
                                }
                                
                                if (fCurrentTab->bot->get_run())
                                    show_noti((char *)texts::noti_reenable_bot, 6000);
							} break;
                        }
						case EN_KILLFOCUS: {
							sprintf(txt_buf, "%.0f", fCurrentTab->bot->get_kill_to_home());
							SetWindowText((HWND)lParam, txt_buf);

                            if (fCurrentTab->bot->get_kill_to_home() == 0) {
                                EnableWindow((HWND)lParam, false);
                                SetWindowText((HWND)lParam, "");
							} break;
                        } break;
					} break;
                }
                case IDC_CHECBKOX_NO_COLLISION: {
                    if (HIWORD(wParam) == BN_CLICKED) {
                        bool checked = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
                        fCurrentTab->localPlayer->set_no_collision(checked);
					} break;
                }
				case ID_CLOSE_TAB: {
					close_tab(TabCtrl_GetCurFocus(hTabControl) -1);
				}/*
				case IDC_CHECBKOX_PERIN_CONVERTER: {
					if (HIWORD(wParam) == BN_CLICKED) {
						bool checked = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
						fCurrentTab->set_perin_convert_spam(checked);

						if (fCurrentTab->run(false))
							show_noti((char *)texts::noti_reenable_bot, 6000);
					} break;
				}*/
				case IDC_EDIT_RESELECT_AFTER: {
					switch (HIWORD(wParam)) {
						case EN_CHANGE: {
							int begin;

							GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
							begin = atoi(txt_buf);

							// if really changed, then only change in class too
							if (fCurrentTab->bot->get_reselect_after() != begin) {
								fCurrentTab->bot->set_reselect_after(begin);

								if (fCurrentTab->bot->get_run())
									show_noti((char *)texts::noti_reenable_bot, 6000);
							} break;
						}
						case EN_KILLFOCUS: {
							int begin;

							begin = fCurrentTab->bot->get_reselect_after();
							sprintf(txt_buf, "%d", begin);
							SetWindowText((HWND)lParam, txt_buf);

							break;
						}
					} break;
				}
				case IDC_COMBO_HP_KEYS: {
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						i = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

						printf("selected f key: %s %x\n", combo_items[i].name, combo_items[i].val);
						fCurrentTab->buff->set_hp_key(flyff::key(combo_items[i].val, 100.f));
					} break;
				}
				case IDC_EDIT_HP: {
					
					switch (HIWORD(wParam)) {
						case EN_CHANGE: {
							int hp_to_buf;

							GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
							hp_to_buf = atoi(txt_buf);

							// if really changed, then only change in class too
							if (fCurrentTab->buff->get_hp_to_buff() != hp_to_buf) {
								fCurrentTab->buff->set_hp_to_buff(hp_to_buf);

								if (hp_to_buf > 0) {
									if (fCurrentTab->buff->get_run() == false) {
										fCurrentTab->buff->run(true);
									}
								} else {
									fCurrentTab->buff->run(false);
								}
							} break;
						}
						case EN_KILLFOCUS: {
							sprintf(txt_buf, "%d", fCurrentTab->buff->get_hp_to_buff());
							SetWindowText((HWND)lParam, txt_buf);

							break;
						} break;
					} break;
				}
                case ID_NOTI: {
                    unsigned char loc[12];
                    
                    GetWindowTextA((HWND)lParam, txt_buf, sizeof(txt_buf) / sizeof(txt_buf[0]));
                    
                    if (strcmp(texts::noti_reenable_bot, txt_buf) == 0) {
                        // noti wants to reeable bot
                        fCurrentTab->bot->stop();
                        fCurrentTab->bot->run();
                    }
                    
                    ShowWindow((HWND)lParam, SW_HIDE);
                    break;
                } break;
			} break;
		}
		
		return true;
	}
	
	return false;
}


INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HWND hwnd;
    char txt_buf[256];

    switch (uMsg) {
		case WM_INITDIALOG: {
			int begin, end, i;
           
            // setting tabs
			INITCOMMONCONTROLSEX ix;
			ix.dwSize = sizeof(INITCOMMONCONTROLSEX);
			ix.dwICC = ICC_TAB_CLASSES;
			InitCommonControlsEx(&ix);
			hTabControl = GetDlgItem(hDlg, IDC_TAB_MAIN);

			add_tab("Select Flyff", IDD_TAB_SELECT_FLYFF);
            
            // show btc addr
            hwnd = GetDlgItem(hDlg, IDC_EDIT_BTC_ADDR);
            SetWindowText(hwnd, STR_BTC_ADDR);

			return true;
		}
		case WM_NOTIFY: {
			switch (((LPNMHDR)lParam)->code) {
                // when tab changed
				case TCN_SELCHANGE: {
					unsigned int index;

					EndDialog(hCurrentTab, 0);
					index = TabCtrl_GetCurFocus(hTabControl);
					printf("tab index: %d\n", index);

					// if tab is not select flyff
					if (index != 0) {
						hCurrentTab = CreateDialog(HIthis, MAKEINTRESOURCE(IDD_TAB_FLYFF), hTabControl, TabDialogProc);
					} else {
						hCurrentTab = CreateDialog(HIthis, MAKEINTRESOURCE(IDD_TAB_SELECT_FLYFF), hTabControl, TabDialogProc);
					}

					return true;
				} return true;
			}
		}
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                /*




                */
            }

            break;
        }
        case WM_CLOSE: {
			DestroyWindow(hDlg);
			return true;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			return true;
		}
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

    // easier to debug errors on windows
    //system("pause");
    return 0;
}
