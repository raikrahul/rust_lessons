use std::env;
use std::fs::File;
use std::io::{self, Read, Write};
use std::process;

const BUF_SIZE: usize = 256;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!("Usage: {} file1 file2", args[0]);
        process::exit(1);
    }

    let mut in_file = match File::open(&args[1]) {
        Ok(file) => file,
        Err(err) => {
            eprintln!("{}: {}", args[1], err);
            process::exit(2);
        }
    };

    let mut out_file = match File::create(&args[2]) {
        Ok(file) => file,
        Err(err) => {
            drop(in_file);
            eprintln!("{}: {}", args[2], err);
            process::exit(3);
        }
    };

    let mut buffer = [0u8; BUF_SIZE];

    loop {
        let bytes_in = match in_file.read(&mut buffer) {
            Ok(n) => n,
            Err(err) => {
                eprintln!("Error reading file: {}", err);
                process::exit(4);
            }
        };

        if bytes_in == 0 {
            break;
        }

        if let Err(err) = out_file.write_all(&buffer[..bytes_in]) {
            eprintln!("Fatal write error: {}", err);
            process::exit(4);
        }
    }
}
