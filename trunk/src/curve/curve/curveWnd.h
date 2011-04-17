#ifndef _CURVEWND_H_
#define _CURVEWND_H_

#include "winmain.h"
#include "curve.h"

#define MARGIN 32
#define LINEHEIGHT 10

struct CurveSpecs {
	TCHAR expr[1024];
	double x1;
	double x2;

	int width;
	double yscale;

	double gridx;
	double gridy;

	TrigMode tm;

	void dump();
};

class CurveWnd {

	double miny;
	double maxy;

	double xsf;	//	scaling factors
	double ysf;	//	ysf is cs->yscale times xsf

	SIZE canvas;
	int gwidth;
	int gheight;
	HPEN gridPen;
	CurveSpecs *cs;

	static TCHAR CLASS_NAME[];
	HWND hwnd;
	Parser p;
	int *ypx;
public:

	CurveWnd();
	static ATOM registerCls();

	HWND create(DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight, HWND hWndParent,
		HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void onPaint(HWND hwnd);
	void onVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void onHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void onSize(HWND hwnd, UINT state, int cx, int cy);
	void onDestroy(HWND hwnd);

	void setPos(int x, int y);
	void setSize(int width, int height);
	void initCurve(CurveSpecs *cs);
	void setGraphSize(int width, int height);

	void resetScroll();

	void setScrollInfo(int bar, int nMin, int nMax, int nPage, int nPos);
	void setScrollPos(int bar, int pos);
	int getScrollPos(int bar);

	HWND getHandle();
	void show(int nShowCmd);
	void update();
	void destroy();
	~CurveWnd();
};

LRESULT CALLBACK curveWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif