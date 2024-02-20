// ProcessList.cpp : Defines the entry point for the console application.
//
#include <Windows.h>
#include <string>
#include "stdafx.h"
#include "resource.h"

using namespace std;

HINSTANCE hInstance;

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    ::hInstance = hInstance;
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hButtonCreateNew, hButtonRefresh, hButtonTerminate, hButtonAdditional, hListBox, hEdit;

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32;
    memset(&pe32, 0, sizeof(PROCESSENTRY32));
    pe32.dwSize = sizeof(PROCESSENTRY32);

    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
            hButtonCreateNew = GetDlgItem(hwnd, IDC_BUTTONCREATE);
            hButtonRefresh = GetDlgItem(hwnd, IDC_BUTTONREFRESH);
            hButtonTerminate = GetDlgItem(hwnd, IDC_BUTTONTERMINATE);
            hButtonAdditional = GetDlgItem(hwnd, IDC_BUTTONADDITIONAL);
            hListBox = GetDlgItem(hwnd, IDC_LIST1);
            hEdit = GetDlgItem(hwnd, IDC_EDIT1);

            SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

            if (Process32First(hSnapShot, &pe32))
            {
                do
                {
                    SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pe32.szExeFile);
                } while (Process32Next(hSnapShot, &pe32));
            }
            return TRUE;
         
        }
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDC_BUTTONREFRESH && HIWORD(wParam) == BN_CLICKED) {
                memset(&pe32, 0, sizeof(PROCESSENTRY32));
                pe32.dwSize = sizeof(PROCESSENTRY32);

                if (Process32First(hSnapShot, &pe32))
                {
                    SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
                    do
                    {
                        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pe32.szExeFile);
                    } while (Process32Next(hSnapShot, &pe32));
                }
            }
            else if (LOWORD(wParam) == IDC_BUTTONTERMINATE && HIWORD(wParam) == BN_CLICKED) {
                int selectedIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                if (selectedIndex != LB_ERR) {
                    TCHAR processName[MAX_PATH];
                    SendMessage(hListBox, LB_GETTEXT, selectedIndex, (LPARAM)processName);

                    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                    PROCESSENTRY32 pe32;
                    pe32.dwSize = sizeof(PROCESSENTRY32);

                    if (Process32First(hSnapshot, &pe32)) {
                        do {
                            if (lstrcmpi(pe32.szExeFile, processName) == 0) {
                                DWORD dwProcessId = pe32.th32ProcessID;
                                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
                                TerminateProcess(hProcess, 0);
                                CloseHandle(hProcess);
                                SendMessage(hListBox, LB_DELETESTRING, selectedIndex, 0);
                            }
                        } while (Process32Next(hSnapshot, &pe32));
                    }

                    CloseHandle(hSnapshot);
                }
                else {
                    MessageBox(hwnd, TEXT("Please select a process to terminate."), TEXT("Error"), MB_OK | MB_ICONERROR);
                }
            }
           else if (LOWORD(wParam) == IDC_BUTTONADDITIONAL && HIWORD(wParam) == BN_CLICKED) {
               int selectedIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
               if (selectedIndex != LB_ERR) {
                   TCHAR processName[MAX_PATH];
                   SendMessage(hListBox, LB_GETTEXT, selectedIndex, (LPARAM)processName);

                   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                   PROCESSENTRY32 pe32;
                   pe32.dwSize = sizeof(PROCESSENTRY32);

                   if (Process32First(hSnapshot, &pe32)) {
                       do {
                           if (lstrcmpi(pe32.szExeFile, processName) == 0) {

                               TCHAR szInfo[MAX_PATH * 2];
                               wsprintf(szInfo, TEXT("Process ID: %lu\nParent Process ID: %lu\nThreads: %lu"), pe32.th32ProcessID, pe32.th32ParentProcessID,pe32.cntThreads);
                               MessageBox(hwnd, szInfo, TEXT("Additional Info"), MB_OK | MB_ICONINFORMATION);

                               break;
                           }
                       } while (Process32Next(hSnapshot, &pe32));
                   }

                   CloseHandle(hSnapshot);
               }
               else {
                   MessageBox(hwnd, TEXT("Please select a process to view additional info."), TEXT("Error"), MB_OK | MB_ICONERROR);
               }
            }
           else if (LOWORD(wParam) == IDC_BUTTONCREATE && HIWORD(wParam) == BN_CLICKED) {
                STARTUPINFO si = { sizeof(STARTUPINFO) };
                PROCESS_INFORMATION pi;
                TCHAR szPath[MAX_PATH] = TEXT("C:\\Windows\\System32\\notepad.exe");

                BOOL fs = CreateProcess(NULL, szPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
                
                if(fs) {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                    MessageBox(hwnd, TEXT("Process created successfully."), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
                }
            }
           else if (LOWORD(wParam) == IDC_EDIT1 && HIWORD(wParam) == EN_CHANGE) {
                TCHAR szProcessName[MAX_PATH];
                GetWindowText(hEdit, szProcessName, MAX_PATH);
                HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                PROCESSENTRY32 pe32;
                pe32.dwSize = sizeof(PROCESSENTRY32);
                int selectedIndex = -1;

                if (Process32First(hSnapShot, &pe32)) {
                    do {
                        if (lstrcmpi(pe32.szExeFile, szProcessName) == 0) {
                            selectedIndex = SendMessage(hListBox, LB_FINDSTRINGEXACT, -1, (LPARAM)pe32.szExeFile);
                        }
                    } while (Process32Next(hSnapShot, &pe32));
                }

                CloseHandle(hSnapShot);

                if (selectedIndex != -1) {
                    SendMessage(hListBox, LB_SETCURSEL, selectedIndex, 0);
                }
            }

            return TRUE;
        }
        case WM_CLOSE:
        {
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;

    }
    return FALSE;
}

    

