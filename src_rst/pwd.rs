
use std::env;
use std::io;
use std::path::PathBuf;

fn main() {
    // TRICK NEUTRALIZER: Leverage Rust's path length abstraction
    let path = env::current_dir().unwrap_or_else(|e| {
        // CRITICAL ERROR DECODE: Mirror C's GetLastError() behavior
        match e.raw_os_error() {
            Some(code) => eprintln!("Error 0x{:x}: Path resolution failed", code),
            None => eprintln!("Unknown error occurred"),
        }
        std::process::exit(1);
    });

    // TRICK DISARMED: Handle trailing slash logic like Windows CMD
    let path_str = path.to_str().unwrap_or_else(|| {
        eprintln!("Error: Invalid UTF-8 in path");
        std::process::exit(1);
    });

    // ANTI-TRICK: Preserve trailing backslash for roots (C:\ vs C:)
    let formatted_path = if path_str.ends_with('\\') && path_str.len() == 3 {
        // Root directory case (e.g., C:\)
        path_str
    } else {
        path_str.trim_end_matches('\\')
    };

    println!("{}", formatted_path);
}