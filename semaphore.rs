// Primitive Semaphore, with a little help from Rust users group
use std::sync::{Condvar, Mutex};

pub struct Semaphore {
    mutex: Mutex<i32>,
    cvar: Condvar,
}

impl Semaphore {
    pub fn new(value: i32) -> Self {
        Semaphore {
            mutex: Mutex::new(value),
            cvar: Condvar::new(),
        }
    }

    pub fn wait(&self) {
        let mut lock = self.mutex.lock().unwrap();
        *lock -= 1;
        if *lock < 0 {
            let _ = self.cvar.wait(lock).unwrap();
        }
    }

    pub fn post(&self) {
        let mut lock = self.mutex.lock().unwrap();
        *lock += 1;
        if *lock <= 0 {
            self.cvar.notify_one();
        }
    }
}

unsafe impl Send for Semaphore {}
unsafe impl Sync for Semaphore {}
