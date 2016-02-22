#include <Windows.h>
#include "resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LPTSTR target;
int showCmd;

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND) {
		if (wParam == IDOK) {
			int length = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_PARAMS)) + 1;
			LPTSTR params = HeapAlloc(GetProcessHeap(), 0, length);
			if (params) {
				GetDlgItemText(hwndDlg, IDC_PARAMS, params, length);
				ShellExecute(hwndDlg, NULL, GetCommandLine(), params, NULL, showCmd);
				HeapFree(GetProcessHeap(), 0, params);
				EndDialog(hwndDlg, IDOK);
				return TRUE;
			} else {
				MessageBox(hwndDlg, TEXT("Out of memory"), TEXT("Error"), MB_ICONERROR);
				return TRUE;
			}
		} else if (wParam == IDCANCEL) {
			EndDialog(hwndDlg, IDCANCEL);
			return TRUE;
		} else {
			return FALSE;
		}
	} else if (uMsg == WM_CLOSE) {
		EndDialog(hwndDlg, IDCANCEL);
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

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, DialogProc);

#ifdef _UNICODE
	HeapFree(GetProcessHeap(), 0, target);
#endif

	return 0;
}