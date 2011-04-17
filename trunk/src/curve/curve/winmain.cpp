#include "stdafx.h"

#include "resource.h"
#include "winmain.h"
#include "MainWnd.h"
#include "curvewnd.h"
#include "controlDlg.h"

HINSTANCE THIS_INSTANCE = NULL;

//	sin(x) + 10 * sin(30 * x)
//	(sin(x) + sin(30 * x)) * (360 - x)
//	(sin(x) + cos(30 * x)) * (360 - x)
//	sin(x) + floor(x)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	MSG msg;
	MainWnd mainWnd;

	THIS_INSTANCE = hInstance;

	if(MainWnd::registerCls() == NULL){
		MessageBox(NULL, _T("Unable to register main window class"), _T("curve"), MB_OK);
		return -1;
	}

	if(mainWnd.create(0, _T("curve - agnel.kurian at gmail.com"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, THIS_INSTANCE, NULL) == NULL){
		MessageBox(NULL, _T("Unable to create main window"), _T("curve"), MB_OK);
		return -1;
	}

	if(CurveWnd::registerCls() == NULL){
		MessageBox(NULL, _T("Unable to register curve window class"), _T("curve"), MB_OK);
		return -1;
	}

	if(mainWnd.createChildren(IDD_CONTROL) == FALSE){
		MessageBox(NULL, _T("Unable to create child windows"), _T("curve"), MB_OK);
		return -1;
	}
	mainWnd.show(nShowCmd);
	mainWnd.update();

	while(GetMessage(&msg, NULL, 0, 0)){
		if(!IsDialogMessage(mainWnd.cd.getHandle(), &msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}