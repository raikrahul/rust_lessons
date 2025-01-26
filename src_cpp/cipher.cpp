#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 4096

BOOL cci_f(LPCSTR fIn, LPCSTR fOut, DWORD shift)
{
    HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE;
    unsigned char aBuffer[BUF_SIZE];  // Changed to unsigned char
    unsigned char ccBuffer[BUF_SIZE]; // Changed to unsigned char and separate declaration
    DWORD nIn = 0, nOut = 0, iCopy = 0;
    BOOL WriteOK = FALSE;

    // First verify input file exists and is readable
    if (GetFileAttributesA(fIn) == INVALID_FILE_ATTRIBUTES) {
        printf("Input file does not exist\n");
        return FALSE;
    }

    hIn = CreateFileA(fIn, GENERIC_READ, FILE_SHARE_READ, NULL, 
                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hIn == INVALID_HANDLE_VALUE) {
        printf("Cannot open input file\n");
        return FALSE;
    }

    hOut = CreateFileA(fOut, GENERIC_WRITE, 0, NULL, 
                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hOut == INVALID_HANDLE_VALUE) {
        CloseHandle(hIn);
        printf("Cannot create output file\n");
        return FALSE;
    }

    // Process file
    while (ReadFile(hIn, aBuffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {
        for (iCopy = 0; iCopy < nIn; iCopy++) {
            ccBuffer[iCopy] = (BYTE)((aBuffer[iCopy] + shift) % 256);
        }

        if (!WriteFile(hOut, ccBuffer, nIn, &nOut, NULL) || nOut != nIn) {
            printf("Write error occurred\n");
            break;
        }

        WriteOK = TRUE;
    }

    // Cleanup
    FlushFileBuffers(hOut);
    CloseHandle(hIn);
    CloseHandle(hOut);

    return WriteOK;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("Usage: %s <input> <output> <shift>\n", argv[0]);
        return 1;
    }

    DWORD shift = (DWORD)atoi(argv[3]);
    
    if (!cci_f(argv[1], argv[2], shift)) {
        printf("Operation failed\n");
        return 1;
    }
    
    printf("File processed successfully\n");
    return 0;
}