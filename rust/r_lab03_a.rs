use std::sync::{Arc, Mutex};
use std::thread;

const NO_TH: usize = 10;
const N_MAX: usize = 50;

fn main() {
    let mut i = 0;
    let counter = Arc::new(Mutex::new(1_i32));
    let mut handles = Vec::with_capacity(NO_TH);
    while i != NO_TH {
        {
            let counter = counter.clone();
            handles.push(thread::spawn(move || {
                let mut c = counter.lock().unwrap();
                for _ in 0..N_MAX {
                    println!("Function 1 = {}; Thread {}", *c, i + 1);
                    *c += 1;
                }
            }));
        }
        i += 1;
        {
            let counter = counter.clone();
            handles.push(thread::spawn(move || {
                let mut c = counter.lock().unwrap();
                for _ in 0..N_MAX {
                    println!("Function 2 = {}; Thread {}", *c, i + 1);
                    *c += 1;
                }
            }));
        }
        i += 1;
    }
    for handle in handles {
        handle.join().unwrap();
    }
}
