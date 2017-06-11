#include "h/gToolTip.h"
//#include "res/resource.h"

#include <stdio.h>

#pragma comment(lib,"comctl32.lib")

bool gToolTip::AddTip(HWND hWnd, HINSTANCE hInst, LPTSTR Tip, UINT id, bool Balloon)
{
    INITCOMMONCONTROLSEX icc;
	HWND hwndTip;
	TOOLINFO		ti;
	icc.dwSize =	sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC =ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_WIN95_CLASSES ;

	InitCommonControlsEx(&icc);

	if(Balloon)//If you have choosen the Boolen Toop Tip will set the Windows style according to that
	{
		hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP |TTS_BALLOON,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, NULL, hInst,
			NULL);
	}
	else
	{

		hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_NOPREFIX |TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, NULL, hInst,
			NULL);
	}

	SendMessage(hwndTip,TTM_ACTIVATE,TRUE,0);              //Will Active the Tool Tip Control

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags =  TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd   = hWnd;							           //Handle of the window in which the Contol resides
	ti.uId    = (UINT)GetDlgItem(hWnd, id);                //ID of the Cotrol for which Tool Tip will be Displyed
	ti.hinst  = hInst;
	ti.lpszText  = Tip;	               					   //Tip you want to Display;
	ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

	if(!SendMessage(hwndTip,TTM_ADDTOOL,0,(LPARAM)&ti)){   //Will add the Tool Tip on Control
		printf("Couldn't create the ToolTip control.\n");
        
        return false;

	}
	
	return true;
}
/*
void  gToolTip::PutInTaskBar(HWND hWndDlg,HINSTANCE hInst,HICON hIcon,UINT TimeOut)
{
	NOTIFYICONDATA nfd;
	nfd.cbSize = sizeof(NOTIFYICONDATA);
	nfd.hWnd	=	hWndDlg;
	wcscpy(nfd.szTip,L"Tip.. " );
	wcscpy(nfd.szInfo, _T("szInfo"));
	wcscpy(nfd. szInfoTitle, _T(" szInfoTitle."));
	nfd.uTimeout = TimeOut*1000;

	//	nfd.uCallbackMessage
	nfd.uFlags =	NIF_ICON | NIF_TIP | NIF_INFO;
	nfd.dwInfoFlags =NIIF_INFO;
	nfd.hIcon  = hIcon;
	if( !Shell_NotifyIcon(NIM_ADD,&nfd))
		MessageBox(NULL,L"Error ",L"" ,0);

}*/
