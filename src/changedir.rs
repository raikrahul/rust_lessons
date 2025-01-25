use std::env;
use std::ffi::OsString;
use std::io::{self, Write};
use std::process;

fn report_error(msg: &str, exit_code: i32, show_error: bool) -> ! {
    eprintln!("Error: {}", msg);
    
    if show_error {
        let error = io::Error::last_os_error();
        eprintln!("System error: {} (code {})", error, error.raw_os_error().unwrap_or(-1));
    }
    
    process::exit(exit_code);
}

fn main() {
    let args: Vec<OsString> = env::args_os().collect();
    let target_dir = if args.len() < 2 {
        // Default to user profile directory
        match env::var_os("USERPROFILE") {
            Some(path) => path,
            None => report_error("Home directory not found.", 1, false),
        }
    } else {
        args[1].clone()
    };

    // Convert OsString to &str for error checking
    let target_str = target_dir.to_str().unwrap_or_else(|| {
        report_error("Invalid target directory encoding.", 1, false)
    });

    if target_str.is_empty() {
        report_error("Invalid target directory.", 1, false);
    }

    if let Err(e) = env::set_current_dir(&target_dir) {
        eprintln!("Failed to change directory to '{}'", target_str);
        report_error(&format!("{}", e), 1, true);
    }

    println!("Current directory is {}", target_str);
}
