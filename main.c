#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#define listenClipboardID 0
#define listenKeystrokesID 1

HHOOK hook = 0;
unsigned char newLineBytes[4] = {13, 0, 10, 0};
unsigned char colonBytes[2] = {58, 0};
unsigned char UTF16LEBOM[2] = {255, 254};

bool exists(char* path) { return fopen(path, "r") != NULL; }

LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam) {
 time_t now = time(0);
 PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
 if(wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
  int key = p->vkCode;
  char ch = MapVirtualKeyA(key, MAPVK_VK_TO_CHAR) != 0 ? MapVirtualKeyA(key, MAPVK_VK_TO_CHAR) : key;
  FILE* file = fopen("keystrokes.txt", "a");
  fprintf(file, "%s: 0x%X %c\n", strtok(ctime(&now), "\n"), key, ch);
  fclose(file);
 }
 return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
 time_t now = time(0);
 switch(Message) {
  case WM_CREATE : {		 
   SetWindowLongA(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);		 
   CreateWindow("BUTTON", "Listen clipboard", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 10, 160, 25, hwnd, (HMENU) listenClipboardID, NULL, NULL);
   CreateWindow("BUTTON", "Listen keystrokes", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 40, 160, 25, hwnd, (HMENU) listenKeystrokesID, NULL, NULL);
   break;
  }
    	 
  case WM_PAINT : {
   PAINTSTRUCT ps;
   HDC hdc = BeginPaint(hwnd, &ps);
   FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
   EndPaint(hwnd, &ps);
   break;
  }
    	    	
  case WM_CTLCOLORSTATIC : {
   SetBkMode((HDC) wParam, TRANSPARENT);
   return (LRESULT) GetStockObject(NULL_BRUSH);
   break;
  }
		
  case WM_CLOSE : {
   ChangeClipboardChain(hwnd, NULL);
   UnhookWindowsHookEx(hook);
   PostQuitMessage(0);
   break;
  }
		
  case WM_COMMAND : {
   if(wParam == listenClipboardID) {
    if(IsDlgButtonChecked(hwnd, listenClipboardID) == BST_UNCHECKED) {
     CheckDlgButton(hwnd, listenClipboardID, BST_CHECKED);
     SetClipboardViewer(hwnd);
    }
    else {
     CheckDlgButton(hwnd, listenClipboardID, BST_UNCHECKED);
     ChangeClipboardChain(hwnd, NULL);
    }
   }
   if(wParam == listenKeystrokesID) {
    if(IsDlgButtonChecked(hwnd, listenKeystrokesID) == BST_UNCHECKED) {
     CheckDlgButton(hwnd, listenKeystrokesID, BST_CHECKED);
     hook = SetWindowsHookEx(WH_KEYBOARD_LL, &keyProc, NULL, 0);
    }
    else {
     CheckDlgButton(hwnd, listenKeystrokesID, BST_UNCHECKED);
     UnhookWindowsHookEx(hook);
    }
   }
   break;
  }
		
  case WM_DRAWCLIPBOARD : {
   if (OpenClipboard(hwnd)) {
    HANDLE clipData = GetClipboardData(CF_UNICODETEXT);
    wchar_t* clipText = (wchar_t*) GlobalLock(clipData);
    GlobalUnlock(clipData);
    CloseClipboard();
    if(clipText != NULL) {
     HANDLE hFile;
     if(!exists("clipboard.txt")) {
      hFile = CreateFile("clipboard.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
      WriteFile(hFile, UTF16LEBOM, 2, NULL, NULL);
      CloseHandle(hFile);
     }
     hFile = CreateFile("clipboard.txt", FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
     char* time = strtok(ctime(&now), "\n");
     wchar_t wTime[strlen(time) + 1];
     mbstowcs(wTime, time, strlen(time) + 1);
     WriteFile(hFile, wTime, 2*wcslen(wTime), NULL, NULL);
     WriteFile(hFile, colonBytes, 2, NULL, NULL);
     WriteFile(hFile, newLineBytes, 4, NULL, NULL);
     WriteFile(hFile, clipText, 2*wcslen(clipText), NULL, NULL);
     WriteFile(hFile, newLineBytes, 4, NULL, NULL);
     CloseHandle(hFile);
    }
   }			
   break; 
  }
			 
  default :
  return DefWindowProc(hwnd, Message, wParam, lParam);
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
 HWND hwnd = CreateWindowEx(0, "WindowClass", "WListener 1.0", WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 250, 100, NULL,NULL,hInstance,NULL);
 if(hwnd == NULL) { return 0; }
 MSG msg = {};
 while(GetMessage(&msg, NULL, 0, 0) > 0) { 
  TranslateMessage(&msg); 
  DispatchMessage(&msg); 
 }
 return 0;
}
