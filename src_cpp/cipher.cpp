#include <windows.h> // (1) Preprocessor directive instructs compiler
                     // to insert windows.h content. Expands to
                     // declarations, potentially inlining functions.
                     // May increase compile time, but avoids
                     // dynamic linking overhead for WinAPI calls.

#include <stdio.h>   // (2) Includes stdio.h. Compiler searches
                     // predefined paths for this header. Provides
                     // buffered I/O functions, potentially using
                     // FILE* struct managed in user space before
                     // system calls for actual I/O operations.

#include <stdlib.h>  // (3) Includes stdlib.h.  Offers general
                     // utilities. `atoi` relies on locale settings
                     // for number parsing, which introduces
                     // potential platform-dependent behavior.

#define BUF_SIZE 4096 // (4) Preprocessor macro substitution. Each
                     // `BUF_SIZE` instance replaced by '4096'
                     // *before* compilation. No runtime cost, but
                     // debugging macro expansions can be harder.
                     // Value is decimal, compiler converts to binary.

BOOL cci_f(LPCSTR fIn, LPCSTR fOut, DWORD shift) // (5) Function declaration. `BOOL` maps to `int`.
                                                 //  `LPCSTR` likely becomes `const char*` under
                                                 //  the hood – pointer passed by value. `DWORD`
                                                 //  is unsigned 32-bit int, ABI-dependent size.

{ // (6) Start function scope. Stack frame allocation
  // begins when function is called. Registers are
  // saved according to calling convention (e.g., x86).

    HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE; // (7) `HANDLE` is opaque pointer type, likely void*.
                                                                   // Initialized to macro, likely -1 or similar
                                                                   // integer bit pattern. Compiler reserves stack
                                                                   // space for two HANDLE variables – sizeof(void*) * 2.

    unsigned char aBuffer[BUF_SIZE];  // (8) `unsigned char` is 1 byte. `aBuffer` is
                                     // statically allocated array on stack, size is
                                     // compile-time constant BUF_SIZE * 1 byte = 4KB.
                                     // Stack pointer adjusted down by 4096 bytes.

    unsigned char ccBuffer[BUF_SIZE]; // (9) Another 4KB stack array. Total stack usage
                                     // for buffers is 8KB within this function.
                                     // Stack overflow possible if stack size is limited
                                     // and BUF_SIZE is very large or nested calls occur.

    DWORD nIn = 0, nOut = 0, iCopy = 0; // (10) Three DWORD (unsigned 32-bit int) variables.
                                         //  Compiler allocates 3 * sizeof(DWORD) bytes
                                         //  on the stack. Zero initialization happens
                                         //  at function entry, potentially using XOR reg, reg.

    BOOL WriteOK = FALSE; // (11) `BOOL` variable, likely `int`. Stack allocation
                          // of sizeof(int) bytes. Initialized to 0 (FALSE).
                          // Simple assignment instruction at function start.

    // First verify input file exists and is readable (12) Comment - compiler ignores. No machine code gen.

    if (GetFileAttributesA(fIn) == INVALID_FILE_ATTRIBUTES) { // (13) `GetFileAttributesA` is WinAPI call. Compiler
        // `GetFileAttributesA(fIn)` (13.1):  Generates call instruction to address of
        //  GetFileAttributesA (resolved at link/load time).
        //  String `fIn` pointer passed as argument in register/stack.
        // `== INVALID_FILE_ATTRIBUTES` (13.2):  Integer comparison. CPU compares return value
        //  register against INVALID_FILE_ATTRIBUTES constant.
        printf("Input file does not exist\n"); // (14) `printf` call. Format string likely placed in
        // .rodata section of executable. String pointer passed.
        return FALSE; // (15) Returns integer 0 (FALSE). Function exit sequence
        // restores saved registers, adjusts stack pointer up.
    } // (16) End of if block. Conditional jump instruction based on comparison.

    hIn = CreateFileA(fIn, GENERIC_READ, FILE_SHARE_READ, NULL, // (17) `CreateFileA` WinAPI. Multiple arguments.
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // Arguments are pushed onto stack or registers
                                                                   // according to calling convention (stdcall likely).
        // `CreateFileA(...)` (17.1): System call overhead. Kernel mode transition.
            // `fIn`: (17.1.1) Pointer to input filename string.
            // `GENERIC_READ`: (17.1.2) Constant, likely inlined by compiler.
            // `FILE_SHARE_READ`: (17.1.3) Constant, inlined.
            // `NULL`: (17.1.4) Null pointer, represented as address 0.
            // `OPEN_EXISTING`: (17.1.5) Constant, inlined.
            // `FILE_ATTRIBUTE_NORMAL`: (17.1.6) Constant, inlined.
            // `NULL`: (17.1.7) Null pointer.

    if (hIn == INVALID_HANDLE_VALUE) { // (18) Handle validity check. Integer comparison.
        printf("Cannot open input file\n"); // (19) `printf` for error message. String literal in .rodata.
        return FALSE; // (20) Return FALSE (0). Function exit sequence.
    } // (21) End if. Conditional jump.

    hOut = CreateFileA(fOut, GENERIC_WRITE, 0, NULL, // (22) Another `CreateFileA` WinAPI call for output.
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Similar argument passing as input file opening.
        // `CreateFileA(...)` (22.1): System call, kernel transition. Overwrites existing file.
            // `fOut`: (22.1.1) Output filename pointer.
            // `GENERIC_WRITE`: (22.1.2) Constant.
            // `0`: (22.1.3) Integer literal 0.
            // `NULL`: (22.1.4) Null pointer.
            // `CREATE_ALWAYS`: (22.1.5) Constant.
            // `FILE_ATTRIBUTE_NORMAL`: (22.1.6) Constant.
            // `NULL`: (22.1.7) Null pointer.

    if (hOut == INVALID_HANDLE_VALUE) { // (23) Output handle validation. Integer comparison.
        CloseHandle(hIn); // (24) `CloseHandle` WinAPI call. Argument hIn passed.
        printf("Cannot create output file\n"); // (25) `printf` error message.
        return FALSE; // (26) Return FALSE (0).
    } // (27) End if. Conditional jump.

    // Process file (28) Comment. Ignored by compiler.

    while (ReadFile(hIn, aBuffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { // (29) `ReadFile` WinAPI in loop condition.
        // `ReadFile(hIn, aBuffer, BUF_SIZE, &nIn, NULL)` (29.1): System call for reading file data.
            // `hIn`: (29.1.1) Input file handle.
            // `aBuffer`: (29.1.2) Pointer to buffer for read data.
            // `BUF_SIZE`: (29.1.3) Number of bytes to read (4096).
            // `&nIn`: (29.1.4) Pointer to DWORD to store bytes read.
            // `NULL`: (29.1.5) Null pointer for overlapped I/O.
        // `&& nIn > 0` (29.2): Logical AND and integer comparison. Loop continues
        // as long as `ReadFile` returns TRUE (non-zero) and nIn is positive.

        for (iCopy = 0; iCopy < nIn; iCopy++) { // (30) `for` loop. `iCopy` incremented and compared.
            // `iCopy = 0; iCopy < nIn; iCopy++` (30.1): Loop initialization, condition, increment.
            // Compiler may unroll loop for small fixed `nIn` (unlikely here).

            ccBuffer[iCopy] = (BYTE)((aBuffer[iCopy] + shift) % 256); // (31) Byte-wise Caesar cipher.
                // `aBuffer[iCopy]` (31.1): Array access. Address calculation: base + iCopy * sizeof(char).
                // `+ shift` (31.2): Integer addition. CPU add instruction.
                // `% 256` (31.3): Modulo operation. Compiler may optimize using bitwise AND if 256 is power of 2.
                // `(BYTE)(...)` (31.4): Explicit type cast. Truncates result to 8 bits.
                // `ccBuffer[iCopy] = ...` (31.5): Array assignment. Memory write operation.

        } // (32) End for loop. Jump back to loop condition check.

        if (!WriteFile(hOut, ccBuffer, nIn, &nOut, NULL) || nOut != nIn) { // (33) `WriteFile` WinAPI to write ciphered data.
            // `WriteFile(hOut, ccBuffer, nIn, &nOut, NULL)` (33.1): System call for writing to file.
                // `hOut`: (33.1.1) Output file handle.
                // `ccBuffer`: (33.1.2) Pointer to ciphered data buffer.
                // `nIn`: (33.1.3) Number of bytes to write.
                // `&nOut`: (33.1.4) Pointer to DWORD to store bytes written.
                // `NULL`: (33.1.5) Null pointer.
            // `!WriteFile(...)` (33.2): Logical NOT of return value. Check for write failure.
            // `|| nOut != nIn` (33.3): Logical OR. Check for partial write. Short-circuiting behavior.

            printf("Write error occurred\n"); // (34) `printf` error message.
            break; // (35) `break` statement. Unconditional jump out of `while` loop.
        } // (36) End if. Conditional jump.

        WriteOK = TRUE; // (37) Set `WriteOK` to TRUE (1). Simple assignment.
    } // (38) End while loop. Jump back to `ReadFile` call if condition met.

    // Cleanup (39) Comment. Ignored.

    FlushFileBuffers(hOut); // (40) `FlushFileBuffers` WinAPI call. Forces OS to write
        // `FlushFileBuffers(hOut)` (40.1): System call to synchronize file buffers.
        // data to disk. Reduces data loss risk on crashes.

    CloseHandle(hIn); // (41) `CloseHandle` WinAPI call. Releases input file handle.
        // `CloseHandle(hIn)` (41.1): System call to release kernel object.

    CloseHandle(hOut); // (42) `CloseHandle` WinAPI call. Releases output file handle.
        // `CloseHandle(hOut)` (42.1): System call to release kernel object.

    return WriteOK; // (43) Return `WriteOK` value (TRUE or FALSE). Function exit.
} // (44) End function scope. Stack frame deallocation. Stack pointer adjusted up.

int main(int argc, char* argv[]) // (45) `main` function entry point. `argc`, `argv` passed
    // `int main(int argc, char* argv[])` (45.1): Standard C++ main signature.
        // `int`: Return type. Exit code to OS.
        // `argc`: Argument count, passed by OS loader.
        // `char* argv[]`: Array of pointers to argument strings, OS provided.

{ // (46) Start main function scope.

    if (argc != 4) { // (47) Argument count check. Integer comparison.
        // `argc != 4` (47.1): Comparison of integer `argc` with constant 4.
        printf("Usage: %s <input> <output> <shift>\n", argv[0]); // (48) `printf` usage message. `argv[0]` program name.
            // `printf(...)`: Output to stdout, buffered. May involve system calls.
            // `%s`: Format specifier, string pointer from `argv[0]` is dereferenced.
        return 1; // (49) Return integer 1, indicating error to OS.
    } // (50) End if. Conditional jump.

    DWORD shift = (DWORD)atoi(argv[3]); // (51) `atoi` call to convert string to integer.
        // `atoi(argv[3])` (51.1): Function call to `atoi` from stdlib. String conversion.
        // `(DWORD)(...)` (51.2): Explicit type cast to DWORD. Potential truncation if result exceeds DWORD range.

    if (!cci_f(argv[1], argv[2], shift)) { // (52) Call to `cci_f` function. Function call overhead.
        // `cci_f(argv[1], argv[2], shift)` (52.1): Function call. Arguments passed (pointers and DWORD).
            // `argv[1]`: Input filename string pointer.
            // `argv[2]`: Output filename string pointer.
            // `shift`: DWORD shift value.
        // `!cci_f(...)` (52.2): Logical NOT of `cci_f` return value. Check for failure.

        printf("Operation failed\n"); // (53) `printf` error message.
        return 1; // (54) Return 1 on `cci_f` failure.
    } // (55) End if. Conditional jump.

    printf("File processed successfully\n"); // (56) `printf` success message.
    return 0; // (57) Return 0, indicating successful program execution.
} // (58) End main function scope. Program termination. OS reclaims resources.