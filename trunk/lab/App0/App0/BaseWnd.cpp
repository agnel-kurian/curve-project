#include "StdAfx.h"
#include "BaseWnd.h"
#include "resource.h"

#include "app0_exception.h"

namespace App0 {

  TCHAR* ERR_CLASS_NOT_REGISTERED = _T("Window class not registered.");
  TCHAR* ERR_CLASS_REGISTRATION_FAILED = _T("Failed to register class.");
  TCHAR* ERR_WINDOW_CREATION_FAILED = _T("Window creation failed.");

  TCHAR* WINDOW_TITLE = _T("Base Window");

  ATOM BaseWnd::wndClassAtom = 0;
  TCHAR* BaseWnd::lpszClassName = _T("{4AEF5BBD-220E-481D-8433-1EB33FFA14CB}");

	BaseWnd::BaseWnd(void): hwnd(0)
	{
    if(wndClassAtom == 0){
      if(Register() == 0)
        throw app0_exception(ERR_CLASS_REGISTRATION_FAILED);
    }

    hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
      MAKEINTATOM(wndClassAtom), WINDOW_TITLE,
      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0,
      GetModuleHandle(0), this);

    if(hwnd == 0)
      throw app0_exception(ERR_WINDOW_CREATION_FAILED);
	}

  BOOL BaseWnd::Show(int nCmdShow){
    return ::ShowWindow(hwnd, nCmdShow);
  }

  BOOL BaseWnd::Update(){
    return UpdateWindow(hwnd);
  }

	BaseWnd::~BaseWnd(void)
	{
	}

  ATOM BaseWnd::Register(){
    WNDCLASSEX wcex;

    wcex.cbClsExtra = wcex.cbWndExtra = 0;
    wcex.cbSize = sizeof(wcex);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    HINSTANCE hInstance = ::GetModuleHandle(NULL);

    wcex.hIcon = (HICON) ::LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP0),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON),
      LR_SHARED);

    wcex.hIconSm = (HICON) ::LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP0),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON),
      ::GetSystemMetrics(SM_CYSMICON), LR_SHARED);

    wcex.hInstance = hInstance;
    wcex.lpfnWndProc = InitWndProc;
    wcex.lpszClassName = lpszClassName;
    wcex.lpszMenuName = NULL;
    wcex.style = CS_HREDRAW | CS_VREDRAW;

    wndClassAtom = ::RegisterClassEx(&wcex);

    return wndClassAtom;
  }

  LRESULT CALLBACK InitWndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam){

    if(msg == WM_CREATE){
      return HANDLE_WM_CREATE(hwnd, wParam, lParam, App0::BaseWnd_OnCreate);
    }
    else
      return ::DefWindowProc(hwnd, msg, wParam, lParam);
  }

  BOOL BaseWnd_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct){
    BaseWnd* window =
      reinterpret_cast<BaseWnd*>((lpCreateStruct->lpCreateParams));

    if(window == 0)
      return FALSE;

    ::SetLastError(0);
    LONG_PTR prevData = ::SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG) window);
    if(prevData != 0)
      return FALSE;
    if(::GetLastError() != 0)
      return FALSE;

    LONG_PTR prev = ::SetWindowLongPtr(hwnd, GWL_WNDPROC,
      (LONG)App0::BaseWndProc);
    if(reinterpret_cast<WNDPROC>(prev) != App0::InitWndProc)
      return FALSE;

    return TRUE;
  }

  LRESULT CALLBACK BaseWndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam){

    return BaseWnd::WndProc(hwnd, msg, wParam, lParam);
  }

  LRESULT BaseWnd::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    LONG_PTR ptr = ::GetWindowLongPtr(hwnd, GWL_USERDATA);
    BaseWnd* window = reinterpret_cast<BaseWnd*>(ptr);
    if(window == 0)
      throw app0_exception(_T("Object pointer is 0."));

    return window->WndProc(msg, wParam, lParam);
  }

  LRESULT BaseWnd::WndProc(UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
      HANDLE_MSG(hwnd, WM_CLOSE, OnWmClose);
      HANDLE_MSG(hwnd, WM_DESTROY, OnWmDestroy);
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
  }

  void BaseWnd::OnWmClose(HWND hwnd){
    ::DestroyWindow(hwnd);
  }

  void BaseWnd::OnWmDestroy(HWND hwnd){
    ::PostQuitMessage(0);
    this->hwnd = 0;
  }
}