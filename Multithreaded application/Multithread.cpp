
#include "header.h"

using namespace std;

INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInstance;
HWND hList, hEdit;

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
	hInstance=hInst;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL,DlgProc);
}
void ProcessList(HWND hList)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	memset(&pe32, 0, sizeof(PROCESSENTRY32));
	pe32.dwSize = sizeof(PROCESSENTRY32);
	wstring proccesName;
	SendMessage(hList,LB_RESETCONTENT, 0, 0);
	if (Process32First(hSnapShot, &pe32))
	{
		proccesName = pe32.szExeFile;
		SendMessage(hList, LB_ADDSTRING, 0, LPARAM(proccesName.c_str()));
		while (Process32Next(hSnapShot, &pe32))
		{
			proccesName = pe32.szExeFile;
			SendMessage(hList, LB_ADDSTRING, 0, LPARAM(proccesName.c_str()));
		}
	}
}
void Terminate(HWND hwnd,wchar_t* name)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	memset(&pe32, 0, sizeof(PROCESSENTRY32));
	pe32.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		if (wcscmp(pe32.szExeFile, name) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			CloseHandle(hSnapShot);
			MessageBox(hwnd, L"Процесс успешно завершен", L"Информация", 0);
			return;
		}

	} while (Process32Next(hSnapShot, &pe32));
	CloseHandle(hSnapShot);
	MessageBox(hwnd, L"Процесс не был найден", L"Ошибка", 0);
}
void AdditionalInfo(HWND hwnd, wchar_t* name)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	memset(&pe32, 0, sizeof(PROCESSENTRY32));
	pe32.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		if (wcscmp(pe32.szExeFile, name) == 0)
		{
			wstring processInfo = L"ID: " + to_wstring(pe32.th32ProcessID) +
				L" Threads: " + to_wstring(pe32.cntThreads) +
				L" Priority: " + to_wstring(pe32.pcPriClassBase) +
				L" Name: " + pe32.szExeFile;
			MessageBox(hwnd, processInfo.c_str(), L"Информация", 0);
			return;
		}

	} while (Process32Next(hSnapShot, &pe32));
	CloseHandle(hSnapShot);
	MessageBox(hwnd, L"Процесс не был найден", L"Ошибка", 0);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	case WM_INITDIALOG:
		hList = GetDlgItem(hwnd, IDC_LIST1);
		hEdit= GetDlgItem(hwnd, IDC_EDIT);
		ProcessList(hList);
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_REFRESH)
		{
			ProcessList(hList);
			return TRUE;
		}
		if (LOWORD(wParam) == IDC_TERMINATE)
		{
			int selectedIndex = SendMessage(hList, LB_GETCURSEL, 0, 0);
			if (selectedIndex != LB_ERR)
			{
				int textLength = SendMessage(hList, LB_GETTEXTLEN, selectedIndex, 0);
				wchar_t* buffer = new wchar_t[textLength + 1];
				SendMessage(hList, LB_GETTEXT, selectedIndex, LPARAM(buffer));
				Terminate(hwnd, buffer);
				delete[] buffer;
				ProcessList(hList);
			}
			else
				MessageBox(hwnd, L"Процесс не выбран!", L"Ошибка", 0);
			return TRUE;
		}
		if (LOWORD(wParam) == IDC_INFO)
		{
			int selectedIndex = SendMessage(hList, LB_GETCURSEL, 0, 0);
			if (selectedIndex != LB_ERR)
			{
				int textLength = SendMessage(hList, LB_GETTEXTLEN, selectedIndex, 0);
				wchar_t* buffer = new wchar_t[textLength + 1];
				SendMessage(hList, LB_GETTEXT, selectedIndex, LPARAM(buffer));
				AdditionalInfo(hwnd, buffer);
				delete[] buffer;
			}
			else
				MessageBox(hwnd, L"Процесс не выбран!", L"Ошибка", 0);
			return TRUE;
		}
		if (LOWORD(wParam) == IDC_START)
		{
			int textLength = GetWindowTextLength(hEdit);
			if (textLength==0)
			{
				MessageBox(hwnd, L"Введите название процесса", L"Ошибка", 0);
				return TRUE;
			}
			wchar_t* buffer = new wchar_t[textLength + 1];
			SendMessage(hEdit, LB_GETTEXT, 0, LPARAM(buffer));
			STARTUPINFO s = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION p;
			BOOL fs = CreateProcess(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &s, &p);
			MessageBox(hwnd, L"Процесс создан", L"Информация", 0);
			if (fs)
			{
				CloseHandle(p.hThread);
				CloseHandle(p.hProcess);
			}
			delete[] buffer;
			return TRUE;
		}
	}
	return FALSE;
}