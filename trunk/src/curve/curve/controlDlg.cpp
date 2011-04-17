#include "stdafx.h"

#include "controldlg.h"
#include "mainwnd.h"
#include "curvewnd.h"

//	lookup table to convert b/w UNITS and "Units"
UnitDesc ControlDlg::angleUnits[] = {
	{ _T("Radians"), RADIANS },
	{ _T("Degrees"), DEGREES },
	{ _T("Grades"), GRADES }
};

ControlDlg::ControlDlg(WORD nTemplateId){
	this->nTemplateId = nTemplateId;
	hdlg = NULL;
}

ControlDlg::~ControlDlg(){
	destroy();
}

void ControlDlg::destroy(){
	if(hdlg != NULL){
		DestroyWindow(hdlg);
		hdlg = NULL;
	}
}

BOOL CALLBACK ControlDlg::dlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	HANDLE_MSG(hdlg, WM_INITDIALOG, onInitDialog);
	HANDLE_MSG(hdlg, WM_COMMAND, onCommand);
	default:
		return FALSE;
	}
}

int ControlDlg::doModal(HWND parent){
	return DialogBoxParam(THIS_INSTANCE, MAKEINTRESOURCE(nTemplateId),
		parent, controlDlgInitProc, (LPARAM) this);
}

void ControlDlg::onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){

	switch(id){
	case ID_ABOUT:
		SendMessage(GetParent(hwnd), UM_CMDABOUT, 0L, 0L);
		break;
	case ID_HELPBTN:
		SendMessage(GetParent(hwnd), UM_CMDHELP, 0L, 0L);
		break;
	case ID_RENDER:
		GetDlgItemText(hdlg, ID_EXPR, cvsp.expr, 1024);	

		cvsp.x1 = getDlgItemDbl(ID_X1);
		cvsp.x2 = getDlgItemDbl(ID_X2);

		cvsp.gridx = getDlgItemDbl(ID_GRIDX);
		cvsp.gridy = getDlgItemDbl(ID_GRIDY);

		cvsp.width = GetDlgItemInt(hwnd, ID_WIDTH, NULL, FALSE);
		cvsp.yscale = getDlgItemDbl(ID_YSCALE);

		int curSel = ComboBox_GetCurSel(GetDlgItem(hwnd, ID_ANGLES));
		cvsp.tm = angleUnits[curSel].tm;
		SendMessage(GetParent(hwnd), UM_CMDRENDER, 0L, 0L);
		break;
	}
}

HWND ControlDlg::create(HWND parent){
	hdlg = CreateDialogParam(THIS_INSTANCE, MAKEINTRESOURCE(nTemplateId),
		parent, controlDlgInitProc, (LPARAM) this);
	return hdlg;
}

void ControlDlg::show(int nShowCmd){
	ShowWindow(hdlg, nShowCmd);
}

BOOL ControlDlg::onInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam){
	int i, n;
	HWND hwndAngles = GetDlgItem(hwnd, ID_ANGLES);
	n = sizeof(angleUnits) / sizeof(angleUnits[0]);
	for(i = 0; i < n; i++)
		ComboBox_AddString(hwndAngles, angleUnits[i].desc);
	SetDlgItemText(hwnd, ID_GRIDX, _T("0"));
	SetDlgItemText(hwnd, ID_GRIDY, _T("0"));
	ComboBox_SetCurSel(hwndAngles, 0);
	return TRUE;
}

void ControlDlg::update(){
	UpdateWindow(hdlg);
}

HWND ControlDlg::getHandle(){
	return hdlg;
}

BOOL CALLBACK controlDlgInitProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam){
	if(msg == WM_INITDIALOG){
		//	Because Redmond works in mysterious ways
		SetWindowLong(hdlg, GWL_USERDATA, (LONG) lParam);
		SetWindowLong(hdlg, DWL_DLGPROC, (LONG) controlDlgProc);
		return controlDlgProc(hdlg, msg, wParam, lParam);
	}
	else
		return FALSE;
}

BOOL CALLBACK controlDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam){
	ControlDlg *cd;
	cd = (ControlDlg *) GetWindowLong(hdlg, GWL_USERDATA);
	return cd->dlgProc(hdlg, msg, wParam, lParam);
}

void ControlDlg::setTemplate(int nTemplateId){
	this->nTemplateId = nTemplateId;
}

void ControlDlg::setSize(int nWidth, int nHeight){
	SetWindowPos(hdlg, NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE);
}

void ControlDlg::setPos(int x, int y){
	SetWindowPos(hdlg, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

double ControlDlg::getDlgItemDbl(int id){
	TCHAR buf[101];	//	MS atof() can handle only 100
	GetDlgItemText(hdlg, id, buf, 101);
	return atof(buf);
}

