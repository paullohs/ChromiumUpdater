#include "stdafx.h"
#include "ChromiumUpdater.h"
#include "Updater.h"
#include <shellapi.h>
#include <Windows.h>
#include <string>
#define MAX_LOADSTRING 100
#define WM_MYMESSAGE (WM_USER + 1)
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK BtnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC origBtnProc = nullptr;
HFONT defaultFont = nullptr;
NOTIFYICONDATA nid;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,  LPWSTR    lpCmdLine, _In_ int       nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_CHROMIUMUPDATER, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);
  if (!InitInstance (hInstance, nCmdShow)) {
    return FALSE;
  }
  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHROMIUMUPDATER));
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = WndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = hInstance;
  wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHROMIUMUPDATER));
  wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)GetStockObject(NULL_BRUSH);
  wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHROMIUMUPDATER);
  wcex.lpszClassName  = szWindowClass;
  wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
  return RegisterClassExW(&wcex);
}

BOOL InitTray(HINSTANCE hInstance, HWND hWnd) {
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hWnd;
  nid.uID = 100;
  nid.uVersion = NOTIFYICON_VERSION;
  nid.uCallbackMessage = WM_MYMESSAGE;
  nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHROMIUMUPDATER));
  wcscpy_s(nid.szTip, L"ChromiumUpdater");
  nid.uFlags = NIF_MESSAGE + NIF_ICON + NIF_TIP;
  return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance;
  HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 365, 170, nullptr, nullptr, hInstance, nullptr);
  if (!hWnd)
    return FALSE;
  defaultFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
    OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI"));
  ChromiumUpdater::windowHandle = hWnd;
  HWND lookupLatest = CreateWindow(L"BUTTON", L"Lookup latest Version", WS_TABSTOP | WS_VISIBLE |  WS_CHILD, 10, 10, 150, 20, hWnd, (HMENU)IDB_LOOKUP_LATEST, nullptr, nullptr);
  HWND latestVersionText = CreateWindow(L"STATIC", L"Latest:", WS_VISIBLE |  WS_CHILD , 10, 40, 50, 15, hWnd, (HMENU)IDS_LATEST_VERSION, nullptr, nullptr);
  HWND latestVersionEdit = CreateWindow(L"EDIT", L"a ", WS_TABSTOP | WS_VISIBLE | WS_DISABLED | WS_CHILD | WS_BORDER, 70, 40, 90, 20, hWnd, (HMENU)IDE_LATEST_VERSION, nullptr, nullptr);
  HWND downloadLatest = CreateWindow(L"BUTTON", L"Download latest", WS_TABSTOP | WS_VISIBLE |  WS_CHILD, 10, 70, 150, 20, hWnd, (HMENU)IDB_DOWNLOAD_LATEST, nullptr, nullptr);
  HWND installLatest = CreateWindow(L"BUTTON", L"Install latest", WS_TABSTOP | WS_VISIBLE |  WS_CHILD, 10, 100, 150, 20, hWnd, (HMENU)IDB_INSTALL_LATEST, nullptr, nullptr);
  EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_INSTALL_LATEST), false);
  HWND lookupCurrent = CreateWindow(L"BUTTON", L"Lookup current Version", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 170, 10, 170, 20, hWnd, (HMENU)IDB_CHECK_INSTALLED, nullptr, nullptr);
  HWND currentVersionText = CreateWindow(L"STATIC", L"Current:", WS_VISIBLE | WS_CHILD, 170, 40, 50, 15, hWnd, (HMENU)IDS_CURRENT_VERSION, nullptr, nullptr);
  HWND currentVersionEdit = CreateWindow(L"EDIT", L"a ", WS_TABSTOP | WS_VISIBLE | WS_DISABLED | WS_CHILD | WS_BORDER, 240, 40, 100, 20, hWnd, (HMENU)IDE_CURRENT_VERSION, nullptr, nullptr);
  HWND checkCB = CreateWindow(L"BUTTON", L"Check hourly for updates", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 170, 70, 170, 20, hWnd, (HMENU)IDCB_CHECK_HOURLY, nullptr, nullptr);
  HWND installCB = CreateWindow(L"BUTTON", L"Install quietly", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 170, 100, 90, 20, hWnd, (HMENU)IDCB_INSTALL_QUIETLY, nullptr, nullptr);
  HWND aboutDia = CreateWindow(L"BUTTON", L"About", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 260, 100, 80, 20, hWnd, (HMENU)IDB_ABOUTDIA, nullptr, nullptr);
  origBtnProc = (WNDPROC)GetWindowLongPtr(lookupCurrent, GWLP_WNDPROC);
  SetWindowLongPtr(lookupLatest, GWLP_WNDPROC, (LONG_PTR)BtnProc);
  SetWindowLongPtr(downloadLatest, GWLP_WNDPROC, (LONG_PTR)BtnProc);
  SetWindowLongPtr(installLatest, GWLP_WNDPROC, (LONG_PTR)BtnProc);
  SetWindowLongPtr(lookupCurrent, GWLP_WNDPROC, (LONG_PTR)BtnProc);
  SetWindowLongPtr(aboutDia, GWLP_WNDPROC, (LONG_PTR)BtnProc);
  InitTray(hInstance, hWnd);
  SetWindowText(latestVersionEdit, L"");
  SetWindowText(currentVersionEdit, L"");
  SendMessage(lookupLatest, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(latestVersionText, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(latestVersionEdit, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(downloadLatest, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(installLatest, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(lookupCurrent, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(currentVersionText, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(currentVersionEdit, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(checkCB, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(installCB, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  SendMessage(aboutDia, WM_SETFONT, (WPARAM)defaultFont, TRUE);
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_MYMESSAGE:
        switch (lParam) {
          case WM_LBUTTONDBLCLK:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            ShowWindow(hWnd, true);
            break;
          default:
            return DefWindowProc(hWnd, message, wParam, lParam);
          }
          break;
    case WM_SYSCOMMAND:
      if ((wParam & 0xFFF0) == SC_MINIMIZE) {
        Shell_NotifyIcon(NIM_ADD, &nid);
        ShowWindow(hWnd, false);
        return 0;
      }
      return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_COMMAND:
      {
        switch (LOWORD(wParam))  {
          case IDCB_CHECK_HOURLY:
            {
              if (HIWORD(wParam) == BN_CLICKED && !SendDlgItemMessage(hWnd, IDCB_CHECK_HOURLY, BM_GETCHECK, 0, 0)) {
                SendMessage(GetDlgItem(ChromiumUpdater::windowHandle, IDCB_CHECK_HOURLY), BM_SETCHECK, BST_CHECKED, 0);
                ChromiumUpdater::autoUpdateEnabled = true;
                if (ChromiumUpdater::loopThreadHandle != NULL) {
                  break;
                } else {
                  ChromiumUpdater::loopThreadHandle = CreateThread(0, 0, LookupLoop, 0, 0, &ChromiumUpdater::loopThreadID);
                }
              } else {
                SendMessage(GetDlgItem(ChromiumUpdater::windowHandle, IDCB_CHECK_HOURLY), BM_SETCHECK, BST_UNCHECKED, 0);
                if (ChromiumUpdater::safetyLock) {
                  SendMessage(GetDlgItem(ChromiumUpdater::windowHandle, IDCB_CHECK_HOURLY), BM_SETCHECK, BST_CHECKED, 0);
                  break;
                }
                ChromiumUpdater::autoUpdateEnabled = false;
                if (ChromiumUpdater::loopThreadHandle != NULL) {
                  TerminateThread(ChromiumUpdater::loopThreadHandle, 0);
                }
                break;
              }
            }
            break;
          case IDCB_INSTALL_QUIETLY:
            {
              if (HIWORD(wParam) == BN_CLICKED && !SendDlgItemMessage(hWnd, IDCB_INSTALL_QUIETLY, BM_GETCHECK, 0, 0)) {
                SendMessage(GetDlgItem(ChromiumUpdater::windowHandle, IDCB_INSTALL_QUIETLY), BM_SETCHECK, BST_CHECKED, 0);
                ChromiumUpdater::quietModeEnabled = true;
              } else {
                SendMessage(GetDlgItem(ChromiumUpdater::windowHandle, IDCB_INSTALL_QUIETLY), BM_SETCHECK, BST_UNCHECKED, 0);
                ChromiumUpdater::quietModeEnabled = false;
              }
            }
            break;
          case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
          default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
      }
      break;
    case WM_CTLCOLORSTATIC:
      SetTextColor((HDC)wParam, RGB(0, 0, 0));
      SetBkMode((HDC)wParam, TRANSPARENT);
      break;
    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
      }
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

LRESULT CALLBACK BtnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_LBUTTONDOWN:
      switch (GetDlgCtrlID(hWnd)) {
      case IDB_ABOUTDIA:
        {
          MessageBox(hWnd, L"Chromium Updater 1.0 \n\u00A9 PL", L"About", MB_OK);
        }
        break;
        case IDB_LOOKUP_LATEST:
          {
            std::wstring text;
            SetDlgItemText(ChromiumUpdater::windowHandle, IDE_LATEST_VERSION, (text = ChromiumUpdater::LookupLatestVersion()).c_str());
            if (!text.empty()) {
              EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_INSTALL_LATEST), false);
              EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_DOWNLOAD_LATEST), true);
            }
          }
          break;
        case IDB_DOWNLOAD_LATEST:
          {
            new std::thread(ChromiumUpdater::DownloadLatestVersionAndHandleButtons);
          }
          break;
        case IDB_INSTALL_LATEST:
          {
            ChromiumUpdater::InstallExeInDirectory();
            EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_INSTALL_LATEST), false);
          }
          break;
        case IDB_CHECK_INSTALLED:
          {
            SetDlgItemText(ChromiumUpdater::windowHandle, IDE_CURRENT_VERSION, ChromiumUpdater::LookupCurrentVersion().c_str());
          }
          break;
      }
      break;
  }
  return CallWindowProc(origBtnProc, hWnd, message, wParam, lParam);
}