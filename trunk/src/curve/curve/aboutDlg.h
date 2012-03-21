#ifndef _ABOUTDLG_H__
#define _ABOUTDLG_H__

#include "winmain.h"

class AboutDlg {
	HWND hwnd;
	int nTemplateId;
public:
	AboutDlg(int nTemplateId = -1);
	~AboutDlg();

	void doModal(HWND parent);
	BOOL onInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	BOOL CALLBACK dlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
	void onCommand(HWND hwnd, int id, HWND hwndCtrl, UINT codeNotify);
};

BOOL CALLBACK aboutDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif //__ABOUTDLG_H__