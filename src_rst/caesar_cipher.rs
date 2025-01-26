use std::env;
use std::fs::{File, OpenOptions};
use std::io::{BufReader, BufWriter, Read, Write};

const BUF_SIZE: usize = 4096;

fn caesar_cipher_file(input_path: &str, output_path: &str, shift: u32) -> std::io::Result<()> {
    // Open input file with buffered reader
    let input_file = File::open(input_path)?;
    let mut reader = BufReader::new(input_file);

    // Open output file with buffered writer (truncate existing or create new)
    let output_file = OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open(output_path)?;
    let mut writer = BufWriter::new(output_file);

    let mut buffer = [0u8; BUF_SIZE];

    loop {
        // Read chunk from input file
        let bytes_read = reader.read(&mut buffer)?;
        if bytes_read == 0 {
            break; // End of file
        }

        // Process each byte in the chunk
        for byte in &mut buffer[..bytes_read] {
            *byte = ((u32::from(*byte) + shift) % 256) as u8;
        }

        // Write processed chunk to output file
        writer.write_all(&buffer[..bytes_read])?;
    }

    // Ensure all data is flushed to disk
    writer.flush()?;
    Ok(())
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() != 4 {
        eprintln!("Usage: {} <input> <output> <shift>", args[0]);
        std::process::exit(1);
    }

    // Parse shift value
    let shift = match args[3].parse::<u32>() {
        Ok(n) => n,
        Err(_) => {
            eprintln!("Error: Shift must be a positive integer");
            std::process::exit(1);
        }
    };

    // Process files
    match caesar_cipher_file(&args[1], &args[2], shift) {
        Ok(_) => println!("File processed successfully"),
        Err(e) => {
            eprintln!("Error processing files: {}", e);
            std::process::exit(1);
        }
    }
}