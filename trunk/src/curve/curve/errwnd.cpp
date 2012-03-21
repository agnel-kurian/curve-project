#include "stdafx.h"

#include "errwnd.h"

ErrWnd::ErrWnd(){
	hwnd = NULL;
}

HWND ErrWnd::create(DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle,
					int x, int y, int width, int height, HWND hwndParent,
					HMENU hmenu, HINSTANCE hInstance, LPVOID lpParam){
	hwnd = CreateWindowEx(dwExStyle, _T("EDIT"), lpWindowName, dwStyle,
		x, y, width, height, hwndParent, hmenu, hInstance, lpParam);
	return hwnd;
		
}

void ErrWnd::destroy(){
	if(hwnd != NULL){
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
}

ErrWnd::~ErrWnd(){
	destroy();
}

void ErrWnd::show(int code){
	ShowWindow(hwnd, code);
}

void ErrWnd::update(){
	UpdateWindow(hwnd);
}

void ErrWnd::setText(TCHAR *text){
	Edit_SetText(hwnd, text);
}

HWND ErrWnd::getHandle(){
	return hwnd;
}