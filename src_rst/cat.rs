
use std::env;
use std::ffi::OsString;
use std::fs::File;
use std::io::{self, Read, Write};

const BUF_SIZE: usize = 512;

fn main() {
    let (dash_s, files) = parse_args();
    
    if files.is_empty() {
        cat_stream(&mut io::stdin(), &mut io::stdout(), dash_s);
        return;
    }

    for path in files {
        match File::open(&path) {
            Ok(mut file) => {
                if let Err(e) = cat_stream(&mut file, &mut io::stdout(), dash_s) {
                    if !dash_s {
                        eprintln!("Error processing file: {}: {}", path.to_string_lossy(), e);
                    }
                }
            }
            Err(e) => {
                if !dash_s {
                    eprintln!("Error opening file: {}: {}", path.to_string_lossy(), e);
                }
            }
        }
    }
}

fn parse_args() -> (bool, Vec<OsString>) {
    let mut dash_s = false;
    let mut files = vec![];
    
    for arg in env::args_os().skip(1) {
        if arg == "-s" {
            dash_s = true;
        } else if arg.to_str().map_or(false, |s| s.starts_with('-')) {
            // Ignore other options
            continue;
        } else {
            files.push(arg);
        }
    }
    
    (dash_s, files)
}

fn cat_stream<R: Read, W: Write>(
    input: &mut R,
    output: &mut W,
    suppress_errors: bool,
) -> io::Result<()> {
    let mut buffer = [0u8; BUF_SIZE];
    
    loop {
        let bytes_read = match input.read(&mut buffer) {
            Ok(0) => break, // EOF
            Ok(n) => n,
            Err(e) if suppress_errors => return Ok(()),
            Err(e) => return Err(e),
        };

        let mut bytes_written = 0;
        while bytes_written < bytes_read {
            let write_result = output.write(&buffer[bytes_written..bytes_read]);
            
            match write_result {
                Ok(n) => bytes_written += n,
                Err(e) if suppress_errors => return Ok(()),
                Err(e) => return Err(e),
            }
        }
    }
    
    Ok(())
}