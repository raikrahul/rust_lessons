use std::fs::{self, File, remove_file};
use std::io::{self, Write, Seek, SeekFrom};
use std::path::Path;

fn main() -> io::Result<()> {
    loop {
        println!("Enter file length in bytes (0 to quit):");
        let mut input = String::new();
        io::stdin().read_line(&mut input)?;
        let file_len: u64 = input.trim().parse().unwrap_or(0);
        
        if file_len == 0 {
            break;
        }

        let file_path = Path::new("temp_test_file");
        let midpoint = file_len / 2;

        report_space("Before file creation")?;

        // Create file
        let mut file = File::create(file_path)?;
        report_space("After file creation")?;

        // Set file length
        file.set_len(file_len)?;
        report_space("After setting file length")?;

        // Write to midpoint
        file.seek(SeekFrom::Start(midpoint))?;
        file.write_all(&[0u8; 256])?;
        report_space("After writing to middle")?;

        // Cleanup
        drop(file);
        remove_file(file_path)?;
        println!("----------------------------------------\n");
    }

    println!("\nEnd of FreeSpace demonstration");
    Ok(())
}

#[cfg(windows)]
fn report_space(stage: &str) -> io::Result<()> {
    use std::ffi::OsStr;
    use std::os::windows::ffi::OsStrExt;
    use std::mem::MaybeUninit;

    #[repr(C)]
    #[derive(Debug)]
    struct ULARGE_INTEGER {
        quad_part: u64,
    }

    extern "system" {
        fn GetDiskFreeSpaceExW(
            lpDirectoryName: *const u16,
            lpFreeBytesAvailableToCaller: *mut ULARGE_INTEGER,
            lpTotalNumberOfBytes: *mut ULARGE_INTEGER,
            lpTotalNumberOfFreeBytes: *mut ULARGE_INTEGER,
        ) -> i32;
    }

    let path = Path::new(".");
    let mut path_utf16: Vec<u16> = OsStr::new(path).encode_wide().chain(Some(0)).collect();

    let mut free_bytes = MaybeUninit::<ULARGE_INTEGER>::uninit();
    let mut total_bytes = MaybeUninit::<ULARGE_INTEGER>::uninit();
    let mut total_free = MaybeUninit::<ULARGE_INTEGER>::uninit();

    let success = unsafe {
        GetDiskFreeSpaceExW(
            path_utf16.as_ptr(),
            free_bytes.as_mut_ptr(),
            total_bytes.as_mut_ptr(),
            total_free.as_mut_ptr(),
        )
    };

    if success == 0 {
        return Err(io::Error::last_os_error());
    }

    let free_bytes = unsafe { free_bytes.assume_init().quad_part };
    let total_bytes = unsafe { total_bytes.assume_init().quad_part };
    let used_space = total_bytes - free_bytes;

    print_space(stage, total_bytes, used_space, free_bytes);
    Ok(())
}

#[cfg(unix)]
fn report_space(stage: &str) -> io::Result<()> {
    use std::os::unix::ffi::OsStrExt;
    use libc::{statvfs, c_char};
    use std::ffi::CString;

    let path = Path::new(".");
    let c_path = CString::new(path.as_os_str().as_bytes()).unwrap();

    let mut stat: libc::statvfs = unsafe { std::mem::zeroed() };
    let result = unsafe { statvfs(c_path.as_ptr() as *const c_char, &mut stat) };

    if result != 0 {
        return Err(io::Error::last_os_error());
    }

    let block_size = stat.f_frsize as u64;
    let total_bytes = stat.f_blocks * block_size;
    let free_bytes = stat.f_bavail * block_size;
    let used_space = total_bytes - free_bytes;

    print_space(stage, total_bytes, used_space, free_bytes);
    Ok(())
}

fn print_space(stage: &str, total: u64, // (1)
                 used: u64,             // (2)
                 free: u64) {           // (3)
    println!("\n{:^20} status:", stage); // (4)
    println!("{:<15} {:>20} bytes",      // (5)
             "Total space:",            // (6)
             format_num(total));        // (7)
    println!("{:<15} {:>20} bytes",      // (8)
             "Used space:",             // (9)
             format_num(used));         // (10)
    println!("{:<15} {:>20} bytes",      // (11)
             "Free space:",             // (12)
             format_num(free));         // (13)
    println!("{:<15} {:>20.2} GB",       // (14)
             "Total:",                  // (15)
             bytes_to_gb(total));       // (16)
    println!("{:<15} {:>20.2} GB",       // (17)
             "Free:",                   // (18)
             bytes_to_gb(free));        // (19)
}

fn format_num(n: u64) -> String {        // (20)
    let num_str = n.to_string();        // (21)
    let mut result = String::new();     // (22)
    let mut count = 0;                  // (23)

    for c in num_str.chars().rev() {    // (24)
        if count % 3 == 0 && count != 0 { // (25)
            result.push(',');           // (26)
        }
        result.push(c);                 // (27)
        count += 1;                     // (28)
    }

    result.chars().rev().collect()      // (29)
}
// Function: print_space
// (1) `u64` occupies 8 bytes in memory,
//     representing unsigned integers.
//     Passed by value, copied to stack.
// (2) `u64` passed to function by value,
//     stack memory allocated for copying.
//     Independent of the original value.
// (3) Another `u64`, copied onto the stack.
//     Separate memory location from 'total'.
//     Copied onto stack for function use.
// (4) Invokes a macro, likely expanded at
//     compile time. Involves I/O, buffered.
//     `stage` is a string slice, typically
//     a pointer and a length.
// (5) Format string processed at compile
//     time, efficient string formatting.
//     Output directed to standard output.
// (6) String literal, stored in the read-
//     only data segment of the executable.
// (7) Calls `format_num`, passing 'total'
//     by value. Return value used in output.
//     Function call overhead incurred.
// (8) Similar to (5). Another format
//     string processed during compilation.
//     Formatting overhead at runtime.
// (9) String literal, potentially inlined
//     by the compiler for optimization.
// (10) `format_num` invoked again, potential
//     cache effects if recently called.
//     Stack memory involved for arg passing.
// (11) Macro invocation, likely involves
//     system calls for writing to stdout.
// (12) String constant, likely stored
//     in the data segment of the program.
// (13) Function call, `free` copied to stack.
//     Function returns a `String` object.
// (14) Format string; `.2` specifies
//     precision, impacts floating-point
//     conversion and output formatting.
// (15) String literal, constant, efficient
//     in terms of memory usage at runtime.
// (16) `bytes_to_gb` call, potentially
//     performs division, type conversion.
//     Result is a float, formatted to 2 dp.
// (17) Macro invocation, output operation
//     is likely buffered for efficiency.
// (18) Constant string used directly without
//     extra allocation during the runtime.
// (19) Function call, return value used in
//     output. Might trigger heap allocation
//     if returns a dynamically sized object.
// Function: format_num
// (20) Function takes `u64` by value.
//     Returns `String` - involves heap
//     allocation for the returned value.
// (21) Converts `u64` to `String`. Involves
//     heap allocation, dynamic memory used.
//     Integer to string conversion logic.
// (22) `String::new()` allocates an empty
//     string on the heap, capacity grows
//     as needed. Initial allocation small.
// (23) `count` initialized on stack. Integer
//     variable, likely stored in register
//     if used frequently in the loop.
// (24) Iterates over string characters in
//     reverse. Involves iterator overhead.
//     Accesses characters potentially UTF-8.
// (25) Integer modulo operation. Branch
//     prediction may be involved for the
//     conditional logic in this segment.
// (26) Pushes a comma to the `String`. May
//     trigger reallocation if capacity
//     is exceeded. Amortized O(1) time.
// (27) Pushes a character to the `String`.
//     String capacity may be adjusted
//     dynamically based on the input.
// (28) Increments `count`. Simple integer
//     addition, likely a single CPU cycle.
//     Stored in a register for quick access.
// (29) Reverses the `String` and collects.
//     Another heap allocation for new string
//     due to reversal. Memory is copied.

fn bytes_to_gb(bytes: u64) -> f64 {
    bytes as f64 / (1024.0 * 1024.0 * 1024.0)
}