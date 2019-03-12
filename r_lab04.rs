use std::sync::{Arc, Mutex};
use std::thread;

const NO_TH: usize = 5;

fn main() {
    let mut handles = Vec::with_capacity(NO_TH);
    let guard = Arc::new(Mutex::new(()));
    for i in 1..=NO_TH {
        let guard = guard.clone();
        handles.push(thread::spawn(move || {
            println!("Airplane {} => Control tower: permission to land", i);
            {
                let _guard = guard.lock().unwrap();
                println!("Control tower = Airplane {}: Permission granted", i);
                println!("Airplane {} => Control tower: I have landed to runway", i);
                println!("Control tower => everyone: Runway is available");
            }
        }));
    }
    for handle in handles {
        handle.join().unwrap();
    }
}
