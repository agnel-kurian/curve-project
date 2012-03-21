#include "stdafx.h"

#include "aboutDlg.h"

AboutDlg::AboutDlg(int nTemplateId){
	this->nTemplateId = nTemplateId;
	hwnd = NULL;
}

AboutDlg::~AboutDlg(){
	if(hwnd != NULL){
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
}

void AboutDlg::doModal(HWND parent){
	DialogBoxParam(THIS_INSTANCE, MAKEINTRESOURCE(nTemplateId), parent, aboutDlgProc, (LPARAM) this);
}

BOOL CALLBACK AboutDlg::dlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	HANDLE_MSG(hwnd, WM_INITDIALOG, onInitDialog);
	HANDLE_MSG(hwnd, WM_COMMAND, onCommand);
	default:
		return FALSE;
	}
}

void AboutDlg::onCommand(HWND hwnd, int id, HWND hwndCtrl, UINT codeNotify){
	if(id == IDOK || id == IDCANCEL)
		EndDialog(hwnd, 0);
}

BOOL AboutDlg::onInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam){
	return TRUE;
}

BOOL CALLBACK aboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	AboutDlg *ad;
	if(msg == WM_INITDIALOG)
		SetWindowLong(hwnd, GWL_USERDATA, (LONG) lParam);

	ad = (AboutDlg*) GetWindowLong(hwnd, GWL_USERDATA);
	return ad->dlgProc(hwnd, msg, wParam, lParam);
}