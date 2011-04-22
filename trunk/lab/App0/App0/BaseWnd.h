#pragma once

namespace App0 {

  extern TCHAR* ERR_CLASS_NOT_REGISTERED;
  extern TCHAR* ERR_CLASS_REGISTRATION_FAILED;
  extern TCHAR* ERR_WINDOW_CREATION_FAILED;

  extern TCHAR* WINDOW_TITLE;

	class BaseWnd
	{
    static ATOM wndClassAtom;

  protected:
    HWND hwnd;
    static TCHAR* lpszClassName;

    static ATOM Register(); 

    virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	public:


		BaseWnd(void);

    void OnWmClose(HWND hwnd);
    void OnWmDestroy(HWND hwnd);

    BOOL Show(int nCmdShow);
    BOOL Update();

		virtual ~BaseWnd(void);

    static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};

  //  initial window proc.
  //  for WM_CREATE, attaches the c++ object to the hwnd and
  //  replaces the wndproc with BaseWndProc
  //  forwards all other messages to DefWindowProc
  LRESULT CALLBACK InitWndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam);

  //  retrieves c++ object pointer from hwnd
  //  forwards all messages to member functions
  LRESULT CALLBACK BaseWndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam);

  BOOL BaseWnd_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
}