#include <stdio.h>
#include <time.h>
#include <windows.h>
#define StartButtonID 0
#define StopButtonID 1
#define ClearConsoleButtonID 2
#define WriteOnFileCheckBoxID 3
#define HideConsoleCheckBoxID 4
#define DeleteFileButtonID 5

BOOL FileExists(LPCTSTR szPath) {
  DWORD dwAttrib = GetFileAttributes(szPath);
  return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    time_t now = time(0);
	switch(Message) {
		
    	case WM_CREATE : {
    		 SetConsoleTitle("Clipboard Listener Version 1.0 Console");
    		 SetWindowLongA(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);		 
    	     CreateWindow("BUTTON", "Start listening", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 110, 25, hwnd, (HMENU) StartButtonID, NULL, NULL);
    	     CreateWindow("BUTTON", "Stop listening", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 150, 10, 110, 25, hwnd, (HMENU) StopButtonID, NULL, NULL);
    	     CreateWindow("BUTTON", "Clear console", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 290, 10, 110, 25, hwnd, (HMENU) ClearConsoleButtonID, NULL, NULL);
    	     CreateWindow("BUTTON", "Delete logs file", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 430, 10, 110, 25, hwnd, (HMENU) DeleteFileButtonID, NULL, NULL);
			 CreateWindow("BUTTON", "Write logs file", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 10, 40, 120, 25, hwnd, (HMENU) WriteOnFileCheckBoxID, NULL, NULL);
 			 CreateWindow("BUTTON", "Hide console", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 150, 40, 110, 25, hwnd, (HMENU) HideConsoleCheckBoxID, NULL, NULL);
			 printf("---%s", ctime (&now));
			 printf("Message: listening disabled");
			 printf("\n\n---%s", ctime (&now));
			 printf("Message: logs file writing disabled");
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
		     if(LOWORD(wParam) == StartButtonID) {
		       SetClipboardViewer(hwnd);
		       printf("\n\n---%s", ctime (&now));
		       printf("Message: listening enabled");
			 }
			 if(LOWORD(wParam) == StopButtonID) {
			   ChangeClipboardChain(hwnd, NULL);
			   printf("\n\n---%s", ctime (&now));
			   printf("Message: listening disabled");
			 }
			 if(LOWORD(wParam) == ClearConsoleButtonID) {
			   system("cls");
			   if(GetClipboardViewer() == hwnd) {
			   	printf("---%s", ctime (&now));
		        printf("Message: listening enabled");
			   }
			   else {
			   	printf("---%s", ctime (&now));
			    printf("Message: listening disabled");
			   }
			   if(IsDlgButtonChecked(hwnd, WriteOnFileCheckBoxID) == BST_CHECKED) {
			   	printf("\n\n---%s", ctime (&now));
		        printf("Message: logs file writing enabled");
			   }
			   else {
			   	printf("\n\n---%s", ctime (&now));
			    printf("Message: logs file writing disabled"); 
			   }
			 }
			 if(LOWORD(wParam) == DeleteFileButtonID) {
			    if(FileExists("CLv1.0Logs.txt")) {
			    	remove("CLv1.0Logs.txt");
			    	printf("\n\n---%s", ctime (&now));
			    	printf("Message: logs file deleted");
				}
			    else {
			    	printf("\n\n---%s", ctime (&now));
			    	printf("Message: logs file not found");
				}
			 }
			 if(LOWORD(wParam) == WriteOnFileCheckBoxID) {
			   if(IsDlgButtonChecked(hwnd, WriteOnFileCheckBoxID) == BST_UNCHECKED) {
			   	CheckDlgButton(hwnd, WriteOnFileCheckBoxID, BST_CHECKED);
			   	printf("\n\n---%s", ctime (&now));
		        printf("Message: logs file writing enabled");
			   }
			   else {
			   	CheckDlgButton(hwnd, WriteOnFileCheckBoxID, BST_UNCHECKED);
			   	printf("\n\n---%s", ctime (&now));
			    printf("Message: logs file writing disabled");
			   }
			 }
			 if(LOWORD(wParam) == HideConsoleCheckBoxID) {
			   if(IsDlgButtonChecked(hwnd, HideConsoleCheckBoxID) == BST_UNCHECKED) {
		        CheckDlgButton(hwnd, HideConsoleCheckBoxID, BST_CHECKED);
		        ShowWindow(GetConsoleWindow(), SW_HIDE);
			   }
			   else {
			   	CheckDlgButton(hwnd, HideConsoleCheckBoxID, BST_UNCHECKED);
		        ShowWindow(GetConsoleWindow(), SW_SHOW);
			   }
			 }
			 break;
		}
		
		 case WM_DRAWCLIPBOARD : {
		 	 if (OpenClipboard(hwnd)) {
               HANDLE ClipData = GetClipboardData(CF_TEXT);
               LPCTSTR clipData = GlobalLock(ClipData);
               GlobalUnlock(ClipData);
               CloseClipboard();
               if(clipData != NULL) {
                printf("\n\n---%s", ctime (&now));
                printf("Listened: %s", clipData);
                if(IsDlgButtonChecked(hwnd, WriteOnFileCheckBoxID) == BST_CHECKED) {
            	    HANDLE hFile = CreateFile("CLv1.0Logs.txt", FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                	LPCTSTR firstPart = "---";
                	unsigned long long int size = strlen(firstPart);
                	WriteFile(hFile, firstPart, size, (LPDWORD) &size, NULL);
                	LPCTSTR secondPart = ctime(&now);
                	size = strlen(secondPart);
                	WriteFile(hFile, secondPart, size, (LPDWORD) &size, NULL);
                	size = strlen(clipData);
                	WriteFile(hFile, clipData, size, (LPDWORD) &size, NULL);
                	LPCTSTR fourthPart = "\n\r";
                	size = strlen(fourthPart);
                	WriteFile(hFile, fourthPart, size, (LPDWORD) &size, NULL);
                	CloseHandle(hFile);
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
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; 
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "WindowClass";

	if(!RegisterClassEx(&wc)) {
		return 0;
	}

	HWND hwnd = CreateWindowEx(0,"WindowClass","Clipboard Listener Version 1.0", WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 555, 100, NULL,NULL,hInstance,NULL);
		                        
	if(hwnd == NULL) {
		return 0;
	}
   
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0) { 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	return 0;
}
