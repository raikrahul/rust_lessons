#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUF_SIZE 512

static void CatFile(HANDLE hIn, HANDLE hOut);
static void ReportError(LPCTSTR msg, DWORD errCode, BOOL showErrMsg);

int _tmain(int argc, TCHAR* argv[]) {
    HANDLE hIn, hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    BOOL dashS = FALSE;
    int iFirstFile = 1;

    /* Flow Step 1a: Parse Options */
    for (int i = 1; i < argc; i++) {
        if (_tcscmp(argv[i], _T("-s")) == 0) {
            dashS = TRUE;
            iFirstFile++;
        }
        else if (argv[i][0] == _T('-')) {
            iFirstFile++;
        }
        else {
            break;
        }
    }

    /* Flow Step 2: Input Source Decision */
    if (iFirstFile >= argc) {
        CatFile(hStdIn, hStdOut);
        return 0;
    }

    /* Flow Step 3: File Processing Loop */
    for (int i = iFirstFile; i < argc; i++) {
        hIn = CreateFile(argv[i], GENERIC_READ, 0, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hIn == INVALID_HANDLE_VALUE) {
            if (!dashS) ReportError(_T("File open error"), GetLastError(), TRUE);
            continue;
        }

        CatFile(hIn, hStdOut);
        
        DWORD err = GetLastError();
        if (err != 0 && !dashS) {
            ReportError(_T("Processing error"), err, TRUE);
        }

        CloseHandle(hIn);
    }
    return 0;
}

static void CatFile(HANDLE hIn, HANDLE hOut) {
    BYTE buffer[BUF_SIZE];
    DWORD nRead, nWritten;

    while (ReadFile(hIn, buffer, BUF_SIZE, &nRead, NULL) && nRead > 0) {
        if (!WriteFile(hOut, buffer, nRead, &nWritten, NULL) || nWritten != nRead) {
            break;
        }
    }
}

static void ReportError(LPCTSTR msg, DWORD errCode, BOOL showErrMsg) {
    _ftprintf(stderr, _T("ERROR: %s"), msg);

    if (showErrMsg) {
        LPVOID errMsg;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, errCode, 0,
            (LPTSTR)&errMsg, 0, NULL);

        _ftprintf(stderr, _T(" (%s)"), (LPTSTR)errMsg);
        LocalFree(errMsg);
    }
    _ftprintf(stderr, _T("\n"));
}
