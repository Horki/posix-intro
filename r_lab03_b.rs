use std::sync::{Arc, Mutex};
use std::thread;

const NO_TH: usize = 2;

fn print_thread(thread: usize) {
    for i in 0..10 {
        match thread {
            0 => println!("Thread I : i = {}", (i * 1000) + 1),
            1 => println!("Thread II : i = {}", (i * 1000) + 1),
            _ => eprintln!("Unkonwn Thread {}", thread),
        }
    }
}

fn main() {
    let mut handles = Vec::with_capacity(NO_TH);
    let guard = Arc::new(Mutex::new(()));
    for i in 0..NO_TH {
        let guard = guard.clone();
        handles.push(thread::spawn(move || {
            for _ in 0..10 {
                let _guard = guard.lock().unwrap();
                print_thread(i);
            }
        }));
    }

    for handle in handles {
        handle.join().unwrap();
    }
}
