mod semaphore;

use semaphore::Semaphore;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;
use std::thread;

const NO_TH: usize = 2;
const N_MAX: usize = 30;

fn main() {
    let mut handles = Vec::with_capacity(NO_TH);
    let sem_odd = Arc::new(Semaphore::new(1));
    let sem_even = Arc::new(Semaphore::new(0));
    let num = Arc::new(AtomicUsize::new(1));
    {
        let sem_even = sem_even.clone();
        let sem_odd = sem_odd.clone();
        let num = num.clone();
        handles.push(thread::spawn(move || loop {
            sem_odd.wait();
            let n = num.fetch_add(1, Ordering::SeqCst);
            println!("Odd number is {}", n);
            sem_even.post();
            if n >= N_MAX - 1 {
                break;
            }
        }));
    }

    handles.push(thread::spawn(move || loop {
        sem_even.wait();
        let n = num.fetch_add(1, Ordering::SeqCst);
        println!("Even number is {}", n);
        sem_odd.post();
        if n >= N_MAX - 1 {
            break;
        }
    }));

    for handle in handles {
        handle.join().unwrap();
    }
}
