#include <Windows.h>
#include "resource.h"

LPTSTR target;
int showCmd;

void MoveWindowToCursor(HWND hWnd);
LPCTSTR CheckShellExecuteResult(int result);

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND) {
		if (wParam == IDOK) {
			int length = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_PARAMS)) + 1;
			LPTSTR params = HeapAlloc(GetProcessHeap(), 0, length);
			if (params) {
				GetDlgItemText(hwndDlg, IDC_PARAMS, params, length);
				
				LPCTSTR msg = CheckShellExecuteResult((int)ShellExecute(hwndDlg, NULL, target, params, NULL, showCmd));
				if (msg) {
					// If msg is not NULL, it means there was an error and it points to an error message.
					MessageBox(hwndDlg, msg, TEXT("Error"), MB_ICONERROR);
				} else {
					PostQuitMessage(0);
				}
				
				HeapFree(GetProcessHeap(), 0, params);
				return TRUE;
			} else {
				MessageBox(hwndDlg, TEXT("Out of memory"), TEXT("Error"), MB_ICONERROR);
				return TRUE;
			}
		} else if (wParam == IDCANCEL) {
			PostQuitMessage(1);
			return TRUE;
		} else {
			return FALSE;
		}
	} else if (uMsg == WM_CLOSE) {
		PostQuitMessage(1);
		return TRUE;
	} else {
		return FALSE;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	showCmd = nCmdShow;

#ifdef _UNICODE
	int bufSize = MultiByteToWideChar(CP_ACP, 0, lpCmdLine, -1, NULL, 0);
	target = HeapAlloc(GetProcessHeap(), 0, bufSize);

	if (!target) {
		MessageBox(NULL, TEXT("Out of memory"), TEXT("Error"), MB_ICONERROR);
		return 255;
	}

	MultiByteToWideChar(CP_ACP, 0, lpCmdLine, -1, target, bufSize);
#else
	target = lpCmdLine;
#endif

	HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, DialogProc);
	MoveWindowToCursor(hWnd);
	ShowWindow(hWnd, SW_SHOWNORMAL);

	MSG msg; while (GetMessage(&msg, hWnd, 0, 0)) {
		if (!IsDialogMessage(hWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	DestroyWindow(hWnd);

#ifdef _UNICODE
	//HeapFree(GetProcessHeap(), 0, target); //Why isn't this working?
#endif

	return 0;
}

void MoveWindowToCursor(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int winWidth = rect.right - rect.left;
	int winHeight = rect.bottom - rect.top;
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	int winX = min(cursorPos.x, GetSystemMetrics(SM_CXSCREEN) - winWidth);
	int winY = min(cursorPos.y, GetSystemMetrics(SM_CYSCREEN) - winHeight);
	MoveWindow(hWnd, winX, winY, winWidth, winHeight, FALSE);
}

LPCTSTR CheckShellExecuteResult(int result)
{
	if (result > 32) {
		return NULL;
	} else {
		switch (result) {
			case 0: case SE_ERR_OOM: return TEXT("The operating system is out of memory or resources.");
			case ERROR_FILE_NOT_FOUND: return TEXT("The specified file was not found.");
			case ERROR_PATH_NOT_FOUND: return TEXT("The specified path was not found.");
			case ERROR_BAD_FORMAT: return TEXT("The specified program is not a valid EXE file.");
			case SE_ERR_ACCESSDENIED: return TEXT("The operating system denied access to the specified file.");
			case SE_ERR_ASSOCINCOMPLETE: return TEXT("The file name association is incomplete or invalid.");
			case SE_ERR_DDEBUSY: return TEXT("The DDE transaction could not be completed because other DDE transactions were being processed.");
			case SE_ERR_DDEFAIL: return TEXT("The DDE transaction failed.");
			case SE_ERR_DDETIMEOUT: return TEXT("The DDE transaction could not be completed because the request timed out.");
			case SE_ERR_DLLNOTFOUND: return TEXT("The specified DLL was not found.");
			case SE_ERR_NOASSOC: return TEXT("There is no application associated with the given file extension.");
			case SE_ERR_SHARE: return TEXT("A sharing violation occurred.");
			default: return TEXT("An unknown error occurred.");
		}
	}
}