#ifndef _CONTROLDLG_H_
#define _CONTROLDLG_H_

#include "winmain.h"
#include "curve.h"
#include "curvewnd.h"

struct UnitDesc {
	TCHAR *desc;
	TrigMode tm;
};

class ControlDlg {
	static UnitDesc angleUnits[];
	HWND hdlg;
	int nTemplateId;
public:
	CurveSpecs cvsp;

	ControlDlg(WORD nTemplateId = -1);
	~ControlDlg();

	void setTemplate(int nTemplateId);
	BOOL CALLBACK dlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
	int doModal(HWND parent);
	HWND create(HWND parent);
	void destroy();

	BOOL onInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	void onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

	void show(int nShowCmd);
	void update();
	void setSize(int width, int height);
	void setPos(int x, int y);

	HWND getHandle();
	double getDlgItemDbl(int id);
};

BOOL CALLBACK controlDlgInitProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK controlDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
#endif