#ifndef _MAINWND_H_
#define _MAINWND_H_

#include "winmain.h"
#include "controldlg.h"
#include "curvewnd.h"
#include "errwnd.h"

#define UM_CMDHELP		(WM_USER + 1)
#define UM_CMDABOUT		(WM_USER + 2)
#define UM_CMDRENDER	(WM_USER + 3)

class MainWnd {
	static TCHAR CLASS_NAME[];
	HWND hwnd;

public:
	ControlDlg cd;
	CurveWnd cw;
	ErrWnd ew;

	MainWnd();
	static ATOM registerCls();

	HWND create(DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight, HWND hWndParent,
		HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	BOOL createChildren(int nTemplateId);
	void onSize(HWND hwnd, UINT state, int cx, int cy);
	void onDestroy(HWND hwnd);

	void onUMHelp();
	void onUMAbout();
	void onUMRender();

	HWND getHandle();
	void show(int nShowCmd);
	void update();
	void destroy();
	~MainWnd();
};

LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif