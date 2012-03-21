#include "stdafx.h"

#include "curvewnd.h"

TCHAR CurveWnd::CLASS_NAME[] = _T("agnel.curve");

void CurveSpecs::dump(){
	printf("%s\n%f to %f\ngrids: %f, %f\nAngle unit: %d\nwidth: %d pixels\ny is %f times x\n",
		expr, x1, x2, gridx, gridy, tm, width, yscale);
}

CurveWnd::CurveWnd(){
	cs = NULL;
	canvas.cx = 1; canvas.cy = 1;
	ypx = NULL;
	hwnd = NULL;
}

CurveWnd::~CurveWnd(){
	destroy();
}

void CurveWnd::destroy(){
	if(hwnd != NULL){
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
}

ATOM CurveWnd::registerCls(){
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(wcex);
	wcex.lpszClassName = CLASS_NAME;
	wcex.cbClsExtra = wcex.cbWndExtra = 0;
	wcex.hInstance = THIS_INSTANCE;
	wcex.lpszMenuName = NULL;
	wcex.lpfnWndProc = DefWindowProc;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hIcon = wcex.hIconSm = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	
	return RegisterClassEx(&wcex);
}

HWND CurveWnd::create(DWORD dwExStyle, LPCTSTR lpszWindowName, DWORD dwStyle,
	int x, int y, int width, int height, HWND hwndParent, HMENU hMenu,
	HINSTANCE hInstance, LPVOID lpParam){
	hwnd = CreateWindowEx(dwExStyle, CLASS_NAME, lpszWindowName, dwStyle,
		x, y, width, height, hwndParent, hMenu, THIS_INSTANCE, lpParam);
	SetWindowLong(hwnd, GWL_USERDATA, (LONG) this);
	SetWindowLong(hwnd, GWL_WNDPROC, (LONG) curveWndProc);

	gridPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	return hwnd;
}

LRESULT CALLBACK CurveWnd::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	HANDLE_MSG(hwnd, WM_SIZE, onSize);
	HANDLE_MSG(hwnd, WM_VSCROLL, onVScroll);
	HANDLE_MSG(hwnd, WM_HSCROLL, onHScroll);
	HANDLE_MSG(hwnd, WM_PAINT, onPaint);
	HANDLE_MSG(hwnd, WM_DESTROY, onDestroy);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

HWND CurveWnd::getHandle(){
	return hwnd;
}

void CurveWnd::onPaint(HWND hwnd){
	PAINTSTRUCT ps;
	int xpos, ypos;
	int save;
	HDC hdc;
	xpos = getScrollPos(SB_HORZ);
	ypos = getScrollPos(SB_VERT);

	hdc = BeginPaint(hwnd, &ps);
	save = SaveDC(hdc);
	if(ypx == NULL){
		EndPaint(hwnd, &ps);
		return;
	}
	SelectObject(hdc, gridPen);

	double gridmax, gridmin;
			double rem;
	if(cs->gridy > 0.0){

		rem = fabs(fmod(maxy, cs->gridy));
		if(fabs(rem - cs->gridy) < 1.0e-10)
			gridmax = maxy;
		else
			gridmax = maxy - rem;

		rem = fabs(fmod(miny, cs->gridy));
		if(rem < 1.0e-10 || fabs(rem - cs->gridy) < 1.0e-10)
			gridmin = miny;
		else
			gridmin = miny + (cs->gridy - rem);

		int maxypx = (int)(maxy * ysf);
		int gridypx;
		for(double y = gridmin; y <= gridmax; y += cs->gridy){
			gridypx = maxypx - (int)(y * ysf);
			MoveToEx(hdc, MARGIN - xpos, MARGIN + gridypx - ypos, NULL);
			LineTo(hdc, gwidth + MARGIN - xpos, MARGIN + gridypx - ypos);
		}
	}

	if(cs->gridx > 0.0){
		rem = fabs(fmod(cs->x2, cs->gridx));
		if(fabs(rem - cs->gridx) < 1.0e-10)
			gridmax = cs->x2;
		else
			gridmax = cs->x2 - rem;

		rem = fabs(fmod(cs->x2, cs->gridx));
		if(rem < 1.0e-10 || fabs(rem - cs->gridx) < 1.0e-10)
			gridmin = cs->x1;
		else
			gridmin = cs->x1 + (cs->gridx - rem);

		int gridxpx;

		for(double x = gridmin; x <= gridmax; x += cs->gridx){
			gridxpx = (int)((x - cs->x1) * xsf);
			MoveToEx(hdc, MARGIN + gridxpx - xpos, MARGIN - ypos, NULL);
			LineTo(hdc, MARGIN + gridxpx - xpos, MARGIN + gheight - ypos);
		}
	}
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	MoveToEx(hdc, MARGIN - xpos, ypx[0] + MARGIN - ypos, NULL);
	for(int xpx = 1; xpx <= gwidth; xpx++)
		LineTo(hdc, xpx + MARGIN - xpos, ypx[xpx] + MARGIN - ypos);
	RestoreDC(hdc, save);
	EndPaint(hwnd, &ps);
}

void CurveWnd::onVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos){
	int nPos, nPage;
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE | SIF_POS;
	GetScrollInfo(hwnd, SB_VERT, &si);

	nPage = si.nPage;
	nPos = si.nPos;

	switch(code){
	case SB_LINEUP:	nPos -= LINEHEIGHT;	break;
	case SB_LINEDOWN: nPos += LINEHEIGHT; break;
	case SB_PAGEUP: nPos -= si.nPage - LINEHEIGHT; break;
	case SB_PAGEDOWN: nPos += si.nPage - LINEHEIGHT; break;
	case SB_THUMBTRACK: case SB_THUMBPOSITION:
		nPos = pos; break;
	}

	if(nPos < 0)
		nPos = 0;

	if(nPos > canvas.cy - nPage)
		nPos = canvas.cy - nPage;

	setScrollPos(SB_VERT, nPos);
	InvalidateRect(hwnd, NULL, TRUE);
}

void CurveWnd::onHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos){
	int nPos, nPage;
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE | SIF_POS;
	GetScrollInfo(hwnd, SB_HORZ, &si);

	nPage = si.nPage;
	nPos = si.nPos;

	switch(code){
	case SB_LINELEFT:	nPos -= LINEHEIGHT;	break;
	case SB_LINERIGHT: nPos += LINEHEIGHT; break;
	case SB_PAGELEFT: nPos -= si.nPage - LINEHEIGHT; break;
	case SB_PAGERIGHT: nPos += si.nPage - LINEHEIGHT; break;
	case SB_THUMBTRACK: case SB_THUMBPOSITION:
		nPos = pos; break;
	}

	if(nPos < 0)
		nPos = 0;

	if(nPos > canvas.cx - nPage)
		nPos = canvas.cx - nPage;

	setScrollPos(SB_HORZ, nPos);
	InvalidateRect(hwnd, NULL, TRUE);
}

void CurveWnd::onDestroy(HWND hwnd){
	DeleteObject(gridPen);
	if(ypx != NULL){
		delete[] ypx;
		ypx = NULL;
	}
}

LRESULT CALLBACK curveWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	CurveWnd *cw;
	cw = (CurveWnd *) GetWindowLong(hwnd, GWL_USERDATA);
	return cw->wndProc(hwnd, msg, wParam, lParam);
}

void CurveWnd::show(int nShowCmd){
	ShowWindow(hwnd, nShowCmd);
}

void CurveWnd::update(){
	UpdateWindow(hwnd);
}

void CurveWnd::setPos(int x, int y){
	SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

void CurveWnd::setSize(int width, int height){
		SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
}

void CurveWnd::setGraphSize(int width, int height){
	canvas.cx = width;
	canvas.cy = height;
}

void CurveWnd::setScrollPos(int bar, int pos){
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = pos;

	SetScrollInfo(hwnd, bar, &si, TRUE);
}

int CurveWnd::getScrollPos(int bar){
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hwnd, bar, &si);
	return si.nPos;
}

void CurveWnd::initCurve(CurveSpecs *cs){
	this->cs = cs;
	p.setTrigMode(cs->tm);
	p.parse(cs->expr);

	if(ypx != NULL){
		delete[] ypx;
		ypx = NULL;
	}

	gwidth = cs->width;

	ypx = new int[gwidth + 1];
	if(ypx == NULL)
		throw ERR_MEMORY;

	int i;
	double x = cs->x1, y;


	maxy = miny = y = p.evaluate(x);
	xsf = gwidth / (cs->x2 - cs->x1);
	ysf = cs->yscale * xsf;
	ypx[0] = (int)(y * cs->yscale * xsf);
	for(i = 1; i <= gwidth; i++){
		y = p.evaluate(cs->x1 + i / xsf);
		if(y < miny) miny = y;
		if(y > maxy) maxy = y;
		ypx[i] = (int)(y * ysf);
	}

	int maxypx, minypx;
	maxypx = (int)(maxy * ysf);
	minypx = (int)(miny * ysf);
	gheight = maxypx - minypx;

	for(i = 0; i <= gwidth; i++)
		ypx[i] = maxypx - ypx[i];
	setGraphSize(gwidth + 2 * MARGIN, gheight + 2 * MARGIN);
	resetScroll();
	InvalidateRect(hwnd, NULL, TRUE);
}

void CurveWnd::resetScroll(){
	RECT cr;

	GetClientRect(hwnd, &cr);

	setScrollInfo(SB_HORZ, 0, canvas.cx, cr.right, 0);
	setScrollInfo(SB_VERT, 0, canvas.cy, cr.bottom, 0);
}

void CurveWnd::onSize(HWND hwnd, UINT state, int cx, int cy){
	int nMax, nPos;

	nPos = getScrollPos(SB_HORZ);
	nMax = canvas.cx - 1;

	if(cx > canvas.cx - nPos){
		if(cx >= canvas.cx)
			nPos = 0;
		else
			nPos = canvas.cx - cx;
	}

	setScrollInfo(SB_HORZ, 0, nMax, cx, nPos);

	nPos = getScrollPos(SB_VERT);
	nMax = canvas.cy - 1;

	if(cy > canvas.cy - nPos){
		if(cy >= canvas.cy)
			nPos = 0;
		else
			nPos = canvas.cy - cy;
	}

	setScrollInfo(SB_VERT, 0, nMax, cy, nPos);
}

void CurveWnd::setScrollInfo(int bar, int nMin, int nMax, int nPage, int nPos){
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.nMax = nMax;
	si.nMin = nMin;
	si.nPage = nPage;
	si.nPos = nPos;
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

	SetScrollInfo(hwnd, bar, &si, TRUE);
}
