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

fn print_space(stage: &str, total: u64, used: u64, free: u64) {
    println!("\n{:^20} status:", stage);
    println!("{:<15} {:>20} bytes", "Total space:", format_num(total));
    println!("{:<15} {:>20} bytes", "Used space:", format_num(used));
    println!("{:<15} {:>20} bytes", "Free space:", format_num(free));
    println!("{:<15} {:>20.2} GB", "Total:", bytes_to_gb(total));
    println!("{:<15} {:>20.2} GB", "Free:", bytes_to_gb(free));
}

fn format_num(n: u64) -> String {
    let num_str = n.to_string();
    let mut result = String::new();
    let mut count = 0;

    for c in num_str.chars().rev() {
        if count % 3 == 0 && count != 0 {
            result.push(',');
        }
        result.push(c);
        count += 1;
    }

    result.chars().rev().collect()
}

fn bytes_to_gb(bytes: u64) -> f64 {
    bytes as f64 / (1024.0 * 1024.0 * 1024.0)
}