#define _WIN32_IE 0x0500
#include <windows.h>
#include "commctrl.h"
#include "tchar.h"


class gToolTip
{
public:
	static bool AddTip(HWND, HINSTANCE, LPTSTR, UINT, bool = false);
	//static void PutInTaskBar(HWND,HINSTANCE,HICON,UINT=INFINITE);
};
