#ifndef _ERRWND_H_
#define _ERRWND_H_

#include "winmain.h"

class ErrWnd {
	HWND hwnd;
public:
	ErrWnd();
	HWND create(DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y,
		int width, int height, HWND hwndParent, HMENU hmenu, HINSTANCE hInstance, LPVOID lpParam);
	void destroy();
	void setText(TCHAR *text);
	void show(int code);
	void update();
	HWND getHandle();
	~ErrWnd();
};

#endif