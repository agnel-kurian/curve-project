#include "stdafx.h"

#include "mainWnd.h"
#include "aboutdlg.h"

TCHAR MainWnd::CLASS_NAME[] = _T("agnel.main");

MainWnd::MainWnd(){
	hwnd = NULL;
}

void MainWnd::destroy(){
	if(hwnd != NULL){
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
}

MainWnd::~MainWnd(){
	destroy();
}

ATOM MainWnd::registerCls(){
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(wcex);
	wcex.cbClsExtra = wcex.cbWndExtra = 0;
	wcex.lpszClassName = CLASS_NAME;
	wcex.hInstance = THIS_INSTANCE;
	wcex.lpszMenuName = NULL;
	wcex.lpfnWndProc = DefWindowProc;	//	we set the actual window procedure later
										//	mail me (agnel.kurian at gmail.com) for more

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = (HICON) LoadImage(THIS_INSTANCE, MAKEINTRESOURCE(IDI_CURVE), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
	wcex.hIconSm = (HICON) LoadImage(THIS_INSTANCE, MAKEINTRESOURCE(IDI_CURVE), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);

	return RegisterClassEx(&wcex);
}


HWND MainWnd::create(DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight, HWND hWndParent,
		HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam){

	hwnd = CreateWindowEx(0, CLASS_NAME, _T("curve  - agnel.kurian at gmail.com"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, THIS_INSTANCE, NULL);
	SetWindowLong(hwnd, GWL_USERDATA, (LONG) this);
	SetWindowLong(hwnd, GWL_WNDPROC, (LONG) mainWndProc);
	return hwnd;

}

LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	MainWnd *mw;

	mw = (MainWnd *) GetWindowLong(hwnd, GWL_USERDATA);
	return mw->wndProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK MainWnd::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	HANDLE_MSG(hwnd, WM_DESTROY, onDestroy);
	HANDLE_MSG(hwnd, WM_SIZE, onSize);
	case UM_CMDABOUT:
		onUMAbout();
		return 0;
	case UM_CMDHELP:
		onUMHelp();
		return 0;
	case UM_CMDRENDER:
		onUMRender();
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

void MainWnd::onSize(HWND hwnd, UINT state, int cx, int cy){
	RECT r;

	GetWindowRect(cd.getHandle(), &r);
	cd.setSize(cx, r.bottom - r.top);

	RECT cr;

	GetClientRect(hwnd, &cr);
	MoveWindow(cw.getHandle(), 0, r.bottom - r.top,
		cr.right - cr.left, cr.bottom - (r.bottom - r.top), TRUE);

	MoveWindow(ew.getHandle(), 0, r.bottom - r.top,
		cr.right - cr.left, r.bottom - r.top, FALSE);

}

void MainWnd::onDestroy(HWND hwnd){
	PostQuitMessage(0);
}

HWND MainWnd::getHandle(){
	return hwnd;
}

void MainWnd::show(int nShowCmd){
	ShowWindow(hwnd, nShowCmd);
}

void MainWnd::update(){
	UpdateWindow(hwnd);
}

BOOL MainWnd::createChildren(int nTemplateId){
	cd.setTemplate(IDD_CONTROL);
	cd.create(hwnd);
	cd.show(SW_SHOW);
	cd.update();

	if(cw.create(0, _T("curve plotter"), WS_CHILD, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		hwnd, NULL, THIS_INSTANCE, NULL) == NULL){
		MessageBox(NULL, _T("Unable to create curve plotter window"), _T("curve"), MB_OK);
		return FALSE;
	}

	cw.show(SW_SHOW);
	cw.update();

	HFONT hFont;

	if(ew.create(0, _T("Errors"), ES_MULTILINE | WS_BORDER | WS_CHILD,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,	hwnd, NULL, THIS_INSTANCE, NULL) == NULL){
		MessageBox(NULL, _T("Unable to create error display window"), _T("curve"), MB_OK);
		return FALSE;
	}

	hFont = GetWindowFont(cd.getHandle());
	SetWindowFont(ew.getHandle(), hFont, FALSE);
	return TRUE;
}

void MainWnd::onUMAbout(){
	AboutDlg ad(IDD_ABOUT);
	ad.doModal(hwnd);
}

void MainWnd::onUMHelp(){
	ShellExecute(hwnd, NULL, _T("curve.htm"), NULL, NULL, SW_MAXIMIZE);
}

void MainWnd::onUMRender(){
	try{
		cw.initCurve(&cd.cvsp);
		ew.show(SW_HIDE);
	}
	catch(int code){
		ew.show(SW_SHOW);
		ew.setText(errMsg[code]);
	}
	catch(ParserError &pe){
		TCHAR buf[1024];
		ew.show(SW_SHOW);
		_stprintf(buf, _T("\t\t%s\r\n\t\t%*s\r\n\t%s\r\n"), pe.expr, pe.pos + 1, _T("^"), errMsg[pe.code]);
		ew.setText(buf);
	}
}
