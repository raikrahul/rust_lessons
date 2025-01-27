#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

void ReportError(LPCTSTR msg, DWORD exitCode, BOOL exitProgram);
void ReportSpace(LPCTSTR Message);

int _tmain(int argc, LPTSTR argv[]) {
    // [1] _tmain: Entry point. Standard for Unicode Win32 console apps.
    // Args: argc (count), argv (values). Unused here.
    // Range: Program execution lifespan. Mem: Stack frame. Comp: O(1).
    // Flow: Program start -> main loop.
    // Narrative: Program begins, initializes, enters file size loop.

    HANDLE hFile;
    // [2] hFile: File handle. Represents file object in OS.
    // Type: HANDLE (void*). Justify: OS file API type.
    // Range: Valid file handle or INVALID_HANDLE_VALUE (-1).
    // Mem: Pointer size (4B or 8B). Comp: O(1) access. Flow: CreateFile -> WriteFile -> CloseHandle.
    // Real-world: File descriptor, like index to OS file table.
    // Edge: INVALID_HANDLE_VALUE. Trigger: CreateFile failure.
    // Response: Error report, program exit. Analyze: Prevents file operations.

    LARGE_INTEGER FileLen, FileLenH;
    // [3] FileLen: Desired file length. 64-bit integer for large files.
    // Type: LARGE_INTEGER (union). Justify: Needs > 4GB file size support.
    // Range: 0 to 2^63 - 1 bytes (max file size). Mem: 8B. Comp: O(1).
    // Flow: Input -> SetFilePointerEx.
    // Real-world: Max file size on NTFS ~ 16EB (exabytes), limited by OS/FS.
    // Edge: Negative input? _tscanf_s behavior. Assume unsigned input effectively.
    // Edge: Very large input? Disk space limit.
    // Flow: User input dictates file size.

    // [4] FileLenH: Half of FileLen. Used for writing to middle.
    // Type: LARGE_INTEGER. Justify: Same type as FileLen for arithmetic.
    // Range: 0 to (2^63 - 1) / 2. Mem: 8B. Comp: O(1). Flow: FileLen -> Calculation -> ov.Offset.
    // Real-world: Midpoint calculation for file write.
    // Calculation: FileLenH = FileLen / 2. Integer division.
    // Pitfall: Integer division truncation. If FileLen is odd, FileLenH is floor(FileLen/2).

    BYTE Buffer[256];
    // [5] Buffer: Write buffer. 256 bytes.
    // Type: BYTE[256] (unsigned char array). Justify: Small write unit, fixed size.
    // Range: 256 bytes. Mem: 256B (stack). Comp: O(1) access. Flow: WriteFile.
    // Real-world: Small chunk of data to write.
    // Size: 256 bytes. Trade-off: Small, but WriteFile can write less.
    // Assume: Buffer content is irrelevant (overwritten file).

    OVERLAPPED ov = { 0 };
    // [6] ov: Overlapped structure for asynchronous file I/O (unused here).
    // Type: OVERLAPPED. Justify: Required for WriteFile even in sync mode if offset used.
    // Mem: OS-dependent struct size (min 24B on x64). Comp: O(1) access. Flow: WriteFile.
    // Roles: Offset control for WriteFile. Initialized to 0 except offset.
    // Hierarchy: struct OVERLAPPED { ... Offset, OffsetHigh ... }. Flat struct.
    // Size: 24B (x64 min). Trade-off: Overhead for offset control.
    // Note: Overlapped I/O not truly used as no async flags set in CreateFile.

    DWORD nWrite;
    // [7] nWrite: Bytes actually written by WriteFile.
    // Type: DWORD (unsigned long, 4B). Justify: WriteFile output parameter type.
    // Range: 0 to 256 (or less if error). Mem: 4B. Comp: O(1). Flow: WriteFile -> unused.
    // Real-world: Count of bytes successfully written to file.
    // Note: Not checked for errors or used after WriteFile call.

    while (1) {
        // [8] while (1): Infinite loop. Continues until user enters 0.
        // Predicate: Always true initially. Invariant: Program runs until user quits.
        // Term: 'break' statement when FileLen.QuadPart == 0.
        // Work: File creation, resizing, writing, cleanup cycle.
        // Pitfall: Infinite loop if 'break' condition never met (but user input ensures termination).
        // Real-world: Program main loop, event loop style (though input driven here).

        FileLen.QuadPart = 0;
        // [9] FileLen.QuadPart = 0: Reset file length at loop start.
        // Operation: Assignment. Comp: O(1). Flow: Loop start -> _tscanf_s.
        // Purpose: Clear previous file size from prior iteration.
        // Invariant: FileLen starts at 0 each loop.

        _tprintf(_T("Enter file length in bytes (0 to quit): "));
        // [10] _tprintf: User prompt for file length input.
        // Operation: Output to console. Comp: O(log N) string length, console I/O. Flow: Output -> User input.
        // Purpose: Guide user interaction.
        // Real-world: User interface text prompt.

        _tscanf_s(_T("%I64d"), &FileLen.QuadPart);
        // [11] _tscanf_s: Reads 64-bit integer from user input into FileLen.QuadPart.
        // Operation: Input from console. Comp: O(log N) input digits, console I/O, parsing. Flow: User input -> FileLen.
        // Predicate: Waits for user input and Enter key.
        // Edge: Non-numeric input? _tscanf_s behavior (error, program might proceed with 0).
        // Edge: Input overflow? (very large number). _tscanf_s behavior (potential overflow or clamping).
        // Assume: User enters valid 64-bit integer or 0 to quit.

        if (FileLen.QuadPart == 0)
            // [12] if (FileLen.QuadPart == 0): Check for user quit condition.
            // Predicate: FileLen.QuadPart == 0. Comp: O(1) comparison. Flow: FileLen -> break/continue.
            // Purpose: Termination condition for loop.
            // Real-world: User command to exit program.

            break;
        // [13] break: Exit the while(1) loop.
        // Operation: Control flow jump. Comp: O(1). Flow: if condition true -> loop exit.
        // Purpose: Terminate program loop when user enters 0.

        _tprintf(_T("\nRequested file size: %,20I64d bytes\n"), FileLen.QuadPart);
        // [14] _tprintf: Echo user input file size to console.
        // Operation: Output to console. Comp: O(log N) number formatting, console I/O. Flow: FileLen -> Output.
        // Purpose: Confirm user input.
        // Format: %,20I64d: comma separators, 20 chars width, 64-bit integer.

        FileLenH.QuadPart = FileLen.QuadPart / 2;
        // [15] FileLenH.QuadPart = FileLen.QuadPart / 2: Calculate half file size.
        // Operation: Integer division. Comp: O(1). Flow: FileLen -> FileLenH.
        // Calculation: Divide FileLen by 2. Result is integer floor.
        // Real-world: Find middle point of file size.

        ReportSpace(_T("Before file creation"));
        // [16] ReportSpace: Report disk space before file creation.
        // Function call: ReportSpace(LPCTSTR). Comp: O(disk I/O) inside ReportSpace. Flow: Call -> ReportSpace -> Return.
        // Purpose: Show disk space before file operation.

        hFile = CreateFile(_T("TempTestFile"), GENERIC_READ | GENERIC_WRITE, 0, NULL,
            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        // [17] CreateFile: Create new file "TempTestFile".
        // API call: Win32 API. Comp: OS dependent, disk I/O, file system operations. Flow: Call -> hFile.
        // Params: File name, access mode (read/write), sharing (none), security, creation (new), attributes, template.
        // Flags: CREATE_NEW: fails if file exists. FILE_ATTRIBUTE_NORMAL: default attributes.
        // Edge: File already exists? CREATE_NEW fails.
        // Edge: Disk full? CreateFile fails.
        // Edge: Permissions issue? CreateFile fails.

        if (hFile == INVALID_HANDLE_VALUE)
            // [18] if (hFile == INVALID_HANDLE_VALUE): Check for CreateFile failure.
            // Predicate: hFile == INVALID_HANDLE_VALUE (-1). Comp: O(1) comparison. Flow: hFile -> error handling.
            // Purpose: Error check after file creation.

            ReportError(_T("Cannot create TempTestFile"), 2, TRUE);
        // [19] ReportError: Report file creation error and exit.
        // Function call: ReportError(LPCTSTR, DWORD, BOOL). Comp: O(error handling) inside ReportError. Flow: Call -> ExitProcess.
        // Params: Error message, exit code 2, exit program TRUE.
        // Exit code 2: Indicates file creation failure.

        ReportSpace(_T("After file creation"));
        // [20] ReportSpace: Report disk space after file creation.
        // Function call: ReportSpace(LPCTSTR). Comp: O(disk I/O) inside ReportSpace. Flow: Call -> ReportSpace -> Return.
        // Purpose: Show disk space after file creation, to see change.

        if (!SetFilePointerEx(hFile, FileLen, NULL, FILE_BEGIN))
            // [21] SetFilePointerEx: Set file pointer to FileLen from beginning.
            // API call: Win32 API. Comp: OS dependent, file system operation. Flow: hFile, FileLen.
            // Params: File handle, distance to move (FileLen), output new pointer (NULL), move method (FILE_BEGIN).
            // Purpose: Prepare to set file size by moving pointer.
            // Edge: Invalid hFile? SetFilePointerEx fails.
            // Edge: FileLen is very large? Disk space? SetFilePointerEx might succeed but SetEndOfFile fail.

            ReportError(_T("Cannot set file pointer"), 3, TRUE);
        // [22] ReportError: Report SetFilePointerEx error and exit.
        // Function call: ReportError(LPCTSTR, DWORD, BOOL). Comp: O(error handling) inside ReportError. Flow: Call -> ExitProcess.
        // Params: Error message, exit code 3, exit program TRUE.
        // Exit code 3: Indicates SetFilePointerEx failure.

        if (!SetEndOfFile(hFile))
            // [23] SetEndOfFile: Set file end to current file pointer position (FileLen).
            // API call: Win32 API. Comp: OS dependent, disk allocation, file system operation. Flow: hFile.
            // Purpose: Allocate space for file up to FileLen.
            // Edge: Disk full? SetEndOfFile fails.
            // Edge: Invalid hFile? SetEndOfFile fails.
            // Edge: Quota limits? SetEndOfFile fails.

            ReportError(_T("Cannot set end of file"), 4, TRUE);
        // [24] ReportError: Report SetEndOfFile error and exit.
        // Function call: ReportError(LPCTSTR, DWORD, BOOL). Comp: O(error handling) inside ReportError. Flow: Call -> ExitProcess.
        // Params: Error message, exit code 4, exit program TRUE.
        // Exit code 4: Indicates SetEndOfFile failure.

        ReportSpace(_T("After setting file length"));
        // [25] ReportSpace: Report disk space after setting file length.
        // Function call: ReportSpace(LPCTSTR). Comp: O(disk I/O) inside ReportSpace. Flow: Call -> ReportSpace -> Return.
        // Purpose: Show disk space after file size allocation.

        ov.Offset = FileLenH.LowPart;
        // [26] ov.Offset = FileLenH.LowPart: Set low 32 bits of file offset in OVERLAPPED struct.
        // Operation: Assignment. Comp: O(1). Flow: FileLenH -> ov.Offset.
        // Purpose: Specify write offset in file (lower part).

        ov.OffsetHigh = FileLenH.HighPart;
        // [27] ov.OffsetHigh = FileLenH.HighPart: Set high 32 bits of file offset in OVERLAPPED struct.
        // Operation: Assignment. Comp: O(1). Flow: FileLenH -> ov.OffsetHigh.
        // Purpose: Specify write offset in file (higher part).
        // Note: Together, ov.Offset and ov.OffsetHigh form 64-bit offset.

        if (!WriteFile(hFile, Buffer, sizeof(Buffer), &nWrite, &ov))
            // [28] WriteFile: Write Buffer to file at offset ov, size sizeof(Buffer).
            // API call: Win32 API. Comp: OS dependent, disk I/O. Flow: hFile, Buffer, ov -> nWrite.
            // Params: File handle, buffer, bytes to write (256), bytes written output (nWrite), overlapped struct (ov).
            // Purpose: Write data to middle of file (at offset FileLenH).
            // Edge: Invalid hFile? WriteFile fails.
            // Edge: Disk full? WriteFile might fail or write less than requested.
            // Edge: Permissions? WriteFile fails.

            ReportError(_T("Cannot write to middle of file"), 5, TRUE);
        // [29] ReportError: Report WriteFile error and exit.
        // Function call: ReportError(LPCTSTR, DWORD, BOOL). Comp: O(error handling) inside ReportError. Flow: Call -> ExitProcess.
        // Params: Error message, exit code 5, exit program TRUE.
        // Exit code 5: Indicates WriteFile failure.

        ReportSpace(_T("After writing to middle"));
        // [30] ReportSpace: Report disk space after writing to middle of file.
        // Function call: ReportSpace(LPCTSTR). Comp: O(disk I/O) inside ReportSpace. Flow: Call -> ReportSpace -> Return.
        // Purpose: Show disk space after write operation.

        CloseHandle(hFile);
        // [31] CloseHandle: Close file handle.
        // API call: Win32 API. Comp: OS dependent, file system operation, resource release. Flow: hFile.
        // Purpose: Release file handle and OS resources.
        // Pitfall: Resource leak if CloseHandle is not called (though RAII handles this in C++ usually, raw handles need manual close).

        DeleteFile(_T("TempTestFile"));
        // [32] DeleteFile: Delete the "TempTestFile".
        // API call: Win32 API. Comp: OS dependent, file system operation. Flow: File deletion.
        // Purpose: Clean up temporary file.
        // Edge: File not found? DeleteFile might fail (though file was just created).
        // Edge: Permissions? DeleteFile fails.

        _tprintf(_T("\n----------------------------------------\n"));
        // [33] _tprintf: Print separator line to console.
        // Operation: Output to console. Comp: O(constant string length), console I/O. Flow: Output.
        // Purpose: Visual separation between loop iterations.

    }
    // [34] }: End of while(1) loop block.

    _tprintf(_T("\nEnd of FreeSpace demonstration\n"));
    // [35] _tprintf: Program completion message.
    // Operation: Output to console. Comp: O(constant string length), console I/O. Flow: Output.
    // Purpose: Indicate program end to user.

    return 0;
    // [36] return 0: Successful program termination.
    // Operation: Return from main function. Comp: O(1). Flow: Program end.
    // Return code 0: Standard for successful exit.

}
// [37] }: End of _tmain function block.

void ReportError(LPCTSTR msg, DWORD exitCode, BOOL exitProgram) {
    // [38] ReportError: Error reporting function.
    // Params: msg (error message), exitCode, exitProgram (flag).
    // Range: Program lifecycle during errors. Mem: Stack frame. Comp: O(error handling). Flow: Error call -> error output -> [exit].
    // Roles: Centralized error reporting.

    LPVOID lpMsgBuf;
    // [39] lpMsgBuf: Buffer to hold formatted error message from FormatMessage.
    // Type: LPVOID (void*). Justify: FormatMessage output buffer type.
    // Mem: Pointer size (4B or 8B). Dynamically allocated by FormatMessage. Flow: FormatMessage -> _tprintf -> LocalFree.
    // Real-world: String buffer for error text.

    DWORD dw = GetLastError();
    // [40] GetLastError: Get last error code from Win32 API.
    // API call: Win32 API. Comp: O(1) (thread-local storage access). Flow: API call -> dw.
    // Purpose: Retrieve error code set by previous API function failure.
    // Range: DWORD (0 to 2^32 - 1), specific error codes.

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    // [41] FormatMessage: Format error message based on error code dw.
    // API call: Win32 API. Comp: OS dependent, string formatting, message lookup. Flow: dw -> lpMsgBuf.
    // Flags: FORMAT_MESSAGE_ALLOCATE_BUFFER: allocate buffer for message. FORMAT_MESSAGE_FROM_SYSTEM: get message from system table.
    // MAKELANGID: Language ID (neutral).
    // Edge: No message for error code? FormatMessage might return generic message or fail.
    // Memory allocation: lpMsgBuf is allocated by FormatMessage.

    _tprintf(_T("\nERROR [%d]: %s\n"), dw, (LPCTSTR)lpMsgBuf);
    // [42] _tprintf: Print formatted error message and error code to console.
    // Operation: Output to console. Comp: O(log N) number formatting, string length, console I/O. Flow: dw, lpMsgBuf -> Output.
    // Purpose: Display error information to user.
    // Format: Error code in [], error message string.

    LocalFree(lpMsgBuf);
    // [43] LocalFree: Free memory allocated by FormatMessage for lpMsgBuf.
    // API call: Win32 API. Comp: O(1) (heap deallocation). Flow: lpMsgBuf.
    // Purpose: Release dynamically allocated memory.
    // Important: Must free memory allocated by FORMAT_MESSAGE_ALLOCATE_BUFFER.

    if (exitProgram) {
        // [44] if (exitProgram): Check if program should exit after reporting error.
        // Predicate: exitProgram is TRUE. Comp: O(1) boolean check. Flow: exitProgram -> ExitProcess.
        // Purpose: Conditional program termination based on error severity.

        ExitProcess(exitCode);
        // [45] ExitProcess: Terminate program immediately with exitCode.
        // API call: Win32 API. Comp: O(1) (process termination). Flow: exitCode -> Program termination.
        // Purpose: Force program exit after critical error.
        // Exit code: Propagates error status to OS/parent process.

    }
    // [46] }: End of if (exitProgram) block.
}
// [47] }: End of ReportError function block.

void ReportSpace(LPCTSTR Message) {
    // [48] ReportSpace: Reports disk space information.
    // Params: Message (string to prefix output).
    // Range: Program lifecycle when disk space is reported. Mem: Stack frame. Comp: O(disk I/O). Flow: ReportSpace call -> space output.
    // Roles: Disk space reporting utility.

    ULARGE_INTEGER FreeBytes, TotalBytes, NumFreeBytes;
    // [49] FreeBytes: Available space to current user.
    // Type: ULARGE_INTEGER (union, 64-bit unsigned). Justify: Needs to represent large disk space values.
    // Range: 0 to 2^64 - 1 bytes (max disk space). Mem: 8B. Comp: O(1). Flow: GetDiskFreeSpaceEx -> _tprintf.
    // Real-world: Disk space user can utilize.

    // [50] TotalBytes: Total disk space.
    // Type: ULARGE_INTEGER. Justify: Same as FreeBytes.
    // Range: 0 to 2^64 - 1 bytes. Mem: 8B. Comp: O(1). Flow: GetDiskFreeSpaceEx -> _tprintf.
    // Real-world: Total capacity of disk volume.

    // [51] NumFreeBytes: Actual free space on disk.
    // Type: ULARGE_INTEGER. Justify: Same as FreeBytes.
    // Range: 0 to 2^64 - 1 bytes. Mem: 8B. Comp: O(1). Flow: GetDiskFreeSpaceEx -> _tprintf.
    // Real-world: Physical free space, might be less than user available due to quotas/permissions.

    const double GB = 1024.0 * 1024.0 * 1024.0;
    // [52] GB: Constant for Gigabyte conversion (1024^3).
    // Type: const double. Justify: Floating point for GB conversion.
    // Value: 1073741824.0 (2^30). Mem: 8B. Comp: O(1). Flow: Used in calculations.
    // Calculation: 1GB = 1024MB, 1MB = 1024KB, 1KB = 1024B. 1024 * 1024 * 1024 = 1073741824.

    if (!GetDiskFreeSpaceEx(NULL, &FreeBytes, &TotalBytes, &NumFreeBytes))
        // [53] GetDiskFreeSpaceEx: Get disk space information for current drive (NULL).
        // API call: Win32 API. Comp: OS dependent, disk I/O. Flow: API call -> FreeBytes, TotalBytes, NumFreeBytes.
        // Params: Drive path (NULL=current), FreeBytes, TotalBytes, NumFreeBytes.
        // Edge: Invalid drive path (NULL usually valid for current). Disk error? GetDiskFreeSpaceEx fails.
        // Returns: BOOL, TRUE on success, FALSE on failure.

        ReportError(_T("Cannot get free space"), 1, TRUE);
    // [54] ReportError: Report GetDiskFreeSpaceEx error and exit.
    // Function call: ReportError(LPCTSTR, DWORD, BOOL). Comp: O(error handling) inside ReportError. Flow: Call -> ExitProcess.
    // Params: Error message, exit code 1, exit program TRUE.
    // Exit code 1: Indicates GetDiskFreeSpaceEx failure.

    _tprintf(_T("\n%25s status:\n"), Message);
    // [55] _tprintf: Print status message with prefix.
    // Operation: Output to console. Comp: O(string length), console I/O. Flow: Message -> Output.
    // Format: %25s: right-align string in 25-char width.

    _tprintf(_T("  Total disk space:   %12.2f GB\n"), (double)TotalBytes.QuadPart / GB);
    // [56] _tprintf: Print total disk space in GB.
    // Operation: Output to console. Comp: O(number formatting, division), console I/O. Flow: TotalBytes, GB -> Output.
    // Calculation: (double)TotalBytes.QuadPart / GB: Convert bytes to GB. Double cast for floating point division.
    // Format: %12.2f: 12 chars width, 2 decimal places, floating point.

    _tprintf(_T("  Actual free space:  %12.2f GB\n"), (double)NumFreeBytes.QuadPart / GB);
    // [57] _tprintf: Print actual free disk space in GB.
    // Operation: Output to console. Comp: O(number formatting, division), console I/O. Flow: NumFreeBytes, GB -> Output.
    // Calculation: (double)NumFreeBytes.QuadPart / GB: Convert bytes to GB.
    // Format: %12.2f: 12 chars width, 2 decimal places, floating point.

    _tprintf(_T("  Available to user:  %12.2f GB\n"), (double)FreeBytes.QuadPart / GB);
    // [58] _tprintf: Print available disk space to user in GB.
    // Operation: Output to console. Comp: O(number formatting, division), console I/O. Flow: FreeBytes, GB -> Output.
    // Calculation: (double)FreeBytes.QuadPart / GB: Convert bytes to GB.
    // Format: %12.2f: 12 chars width, 2 decimal places, floating point.

}
// [59] }: End of ReportSpace function block.