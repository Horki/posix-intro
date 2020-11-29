mod semaphore;

use semaphore::Semaphore;
use std::sync::Arc;
use std::thread;

const NO_TH: usize = 5;

fn main() {
    let mut handles = Vec::with_capacity(NO_TH);
    let sem = Arc::new(Semaphore::new(1));
    for i in 1..=NO_TH {
        let sem = sem.clone();
        handles.push(thread::spawn(move || {
            println!("Airplane {} => Control tower: permission to land", i);
            sem.wait();
            println!("Control tower = Airplane {}: Permission granted", i);
            println!("Airplane {} => Control tower: I have landed to runway", i);
            println!("Control tower => everyone: Runway is available");
            sem.post();
        }));
    }
    for handle in handles {
        handle.join().unwrap();
    }
}
