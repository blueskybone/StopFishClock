#include <windows.h>
#include <shellapi.h>
#include <WinUser.h>
#include <tchar.h>
#include<string.h>
#include<string>
#include "bass.h" 
#include "resource.h"
#define IDB_ONE     3301  

#define IDM_EXT WM_USER+301
#define IDM_SEC WM_USER+302
#define IDM_THI WM_USER+303

#define WM_SHOWTASK WM_USER+101 
#define WM_ONCLOSE WM_USER+102
#define PATH "sound.mp3"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
HSTREAM *strs = NULL;
HWND win = NULL;
HWND hwndbutton;
UINT TimerId = NULL;

static HHOOK hhkMouse = NULL;
static OPENFILENAME ofn = { 0 };
/* This is where all the input to the window goes to */

VOID CALLBACK myTimerProc1(
	HWND hwnd, // handle of window for timer messages
	UINT uMsg, // WM_TIMER message
	UINT idEvent, // timer identifier
	DWORD dwTime // current system time
	) {
	//MessageBox(NULL, TEXT("Timer test1"), TEXT("timer test1"), MB_ICONEXCLAMATION | MB_OK);
	//TODO:
	HSTREAM str;
	TCHAR file[MAX_PATH] = { 0 };
	strcpy_s(file, MAX_PATH, PATH);
	GetShortPathName(file, file, 50);
	str = BASS_StreamCreateFile(FALSE, file, 0, 0, 0);
	if (!BASS_ChannelPlay(str, FALSE)) // play the stream (continue from current position)
		MessageBox(NULL, TEXT("ERROR"), TEXT("CAN‘T PLAY STREAM"), MB_ICONEXCLAMATION | MB_OK);

	//MessageBox(NULL, TEXT("Never gonna give you up"), TEXT("never gonna let you down"), MB_ICONEXCLAMATION | MB_OK);
	KillTimer(NULL, TimerId);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	static PMSLLHOOKSTRUCT mouseHookStruct;
	if (nCode == HC_ACTION)
	{

		mouseHookStruct = (PMSLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_MOUSEMOVE:
		{
							 if (TimerId != NULL)KillTimer(NULL, TimerId);
							 TimerId = SetTimer(NULL, 0, 30000, myTimerProc1);				 // do something

							 break;
		}
		default:
			break;
		}
	}

	return CallNextHookEx(hhkMouse, nCode, wParam, lParam);
}
BOOL WINAPI StartHookMouse()
{
	if (NULL == hhkMouse)
	{
		hhkMouse = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
		if (NULL == hhkMouse)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	return TRUE;
}
VOID WINAPI StopHookMouse()
{
	if (hhkMouse != NULL)
	{
		::UnhookWindowsHookEx(hhkMouse);
	}
}

VOID ToTray(HWND hwnd)
{
	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	nid.hWnd = hwnd;
	lstrcpy(nid.szTip, _T("PRTS-TIMER"));
	nid.uCallbackMessage = WM_SHOWTASK;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &nid);//在托盘区添加图标

}
void DeleteTray(HWND hwnd)
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	//nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;
	nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	lstrcpy(nid.szTip, _T("PRTS-TIMER"));
	Shell_NotifyIcon(NIM_DELETE, &nid);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {



	switch (Message) {

		/* Upon destruction, tell the main thread to stop */
	case WM_DESTROY: {
						 PostQuitMessage(0);
						 StopHookMouse();
						 DeleteTray(hwnd);
						 DestroyWindow(hwnd);
						 KillTimer(NULL, TimerId);
						 break;
	}
	case WM_CLOSE: {

					   break;
	}
	case WM_CREATE:{
					   hwndbutton = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"),
						   TEXT("choose"),
						   WS_CHILD | WS_VISIBLE,
						   30, 30, 80, 30,
						   hwnd, (HMENU)IDB_ONE,
						   ((LPCREATESTRUCT)lParam)->hInstance, NULL);

					   ofn.lStructSize = sizeof(OPENFILENAME);
					   ofn.hwndOwner = NULL;
					   ofn.lpstrInitialDir = NULL;
					   ofn.nFilterIndex = 1;
					   ofn.lpstrTitle = TEXT("choose a file");
					   ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;


					   break;
	}
	case WM_SIZE:{
					 if (wParam == SIZE_MINIMIZED)
					 {
						 ToTray(hwnd);
						 ShowWindow(hwnd, SW_HIDE);
					 }
					 break;
	}
	case WM_SHOWTASK:{
						 if (lParam == WM_LBUTTONDOWN)
						 {
							 SetForegroundWindow(hwnd);
							 ShowWindow(hwnd, SW_SHOWNORMAL);
						 }
						 if (lParam == WM_RBUTTONDOWN)
						 {
							 POINT pt;
							 GetCursorPos(&pt);
							 HMENU hMenu = CreatePopupMenu();
							 AppendMenu(hMenu, MF_STRING, IDM_SEC, TEXT("退出"));
							 SetForegroundWindow(hwnd);
							 int cmd = TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, NULL, hwnd, NULL);
							 switch (cmd)
							 {
							 case IDM_SEC:
							 {
											 //MessageBox(NULL, TEXT("TESTING"), TEXT("test"), MB_ICONEXCLAMATION | MB_OK);
											 SendMessage(hwnd, WM_DESTROY, 0, 0);
											 break;
							 }
							 }

						 }
						 break;
	}
	case WM_COMMAND:{

						switch (LOWORD(wParam)){

						case IDB_ONE:
						{
										HSTREAM str;
										TCHAR file[MAX_PATH] = { 0 };
										ofn.lpstrFilter = TEXT("Streamable files (wav/aif/mp3/mp2/mp1/ogg)\0*.wav;*.aif;*.mp3;*.mp2;*.mp1;*.ogg\0All files\0*.*\0\0");
										ofn.lpstrFile = file;
										ofn.nMaxFile = sizeof(file);

										if (GetOpenFileName(&ofn))
										{
											str = BASS_StreamCreateFile(FALSE, file, 0, 0, 0);
											if (!BASS_ChannelPlay(str, FALSE)) // play the stream (continue from current position)
												MessageBox(NULL, TEXT("ERROR"), TEXT("CAN‘T PLAY STREAM"), MB_ICONEXCLAMATION | MB_OK);

											//MessageBox(NULL, file, TEXT("choosed file:"), 0);
										}
										break;
						}

						}
						break;

						/* All other messages (a lot of them) are processed using default procedures */
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
		return 0;
	}
}



/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	HMENU hmenu;
	MSG msg; /* A temporary location for all messages */

	// enable "Default" device that follows default device changes
	BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1);
	if (!BASS_Init(-1, 44100, 0, win, NULL)) {
		MessageBox(NULL, TEXT("Can't initialize device"), TEXT("Can't initialize device"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if (!StartHookMouse()){
		MessageBox(NULL, TEXT("hookmouse install failed!"), TEXT("hookmouse install failed!"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	else {
		//MessageBox(NULL, TEXT("hookmouse install success!"), TEXT("hookmouse install success!"), MB_ICONEXCLAMATION | MB_OK);
	}

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc; /* This is where we will send messages to */
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = TEXT("WindowClass");
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("WindowClass"), TEXT("PRTS"), WS_SIZEBOX | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		400, /* width */
		200, /* height */
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) {
		MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	hmenu = GetSystemMenu(hwnd, false);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	ToTray(hwnd);
	/*
	This is the heart of our program where all input is processed and
	sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
	this loop will not produce unreasonably high CPU usage
	*/
	while (GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}