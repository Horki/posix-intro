mod semaphore;

use semaphore::Semaphore;
use std::cell::Cell;
use std::sync::{Arc, Mutex};
use std::{fmt, thread, time};

const BUFFER_LEN: usize = 10;
const NO_TH: usize = 5;

#[derive(Debug, Clone, Copy)]
enum Sloth {
    Empty,
    Taken(Item),
    Deleted,
}

#[derive(Debug, Clone, Copy)]
enum Item {
    Hat,
    Phone,
    Dress,
    Pants,
    Shoes,
    Burrito,
    Sushi,
    Sandals,
    PC,
    Other,
}

impl fmt::Display for Item {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let res = match *self {
            Item::Hat => "hat",
            Item::Phone => "phone",
            Item::Dress => "dress",
            Item::Pants => "pants",
            Item::Shoes => "shoes",
            Item::Burrito => "burrito",
            Item::Sushi => "sushi",
            Item::Sandals => "sandals",
            Item::PC => "pc",
            Item::Other => "other",
        };
        write!(f, "{}", res)
    }
}

struct Buffer {
    sloths: Vec<Cell<Sloth>>,
    len: usize,
    producer_idx: Cell<usize>,
    consumer_idx: Cell<usize>,
}

impl Buffer {
    fn new(len: usize) -> Self {
        Self {
            sloths: vec![Cell::new(Sloth::Empty); len],
            len,
            producer_idx: Cell::new(0),
            consumer_idx: Cell::new(0),
        }
    }

    fn add(&self, item: Item) {
        self.sloths[self.producer_idx.get()].set(Sloth::Taken(item));
        self.producer_idx
            .set((self.producer_idx.get() + 1) % self.len);
    }

    fn remove(&self) -> Item {
        let sloth = self.sloths[self.consumer_idx.get()].get();
        let item = match sloth {
            Sloth::Taken(item) => item,
            _ => Item::Other,
        };
        self.sloths[self.consumer_idx.get()].set(Sloth::Deleted);
        self.consumer_idx
            .set((self.consumer_idx.get() + 1) % self.len);
        item
    }
}

impl fmt::Display for Buffer {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let mut res = String::new();
        res.push('[');
        for i in 0..self.len {
            match self.sloths[i].get() {
                Sloth::Empty => res.push('0'),
                Sloth::Taken(_) => res.push('*'),
                Sloth::Deleted => {
                    res.push('+');
                    self.sloths[i].set(Sloth::Empty);
                }
            }
        }
        res.push(']');
        write!(f, "{}", res)
    }
}

fn consume(consumer_id: i32, item: Item) {
    eprintln!("Worker [{}] sending {} to cosumer", consumer_id, item);
}

fn produce() -> Item {
    let rnd = (time::SystemTime::now()
        .duration_since(time::SystemTime::UNIX_EPOCH)
        .unwrap()
        .as_micros()
        % 9)
        + 1;
    match rnd {
        1 => Item::Hat,
        2 => Item::Phone,
        3 => Item::Dress,
        4 => Item::Pants,
        5 => Item::Shoes,
        6 => Item::Burrito,
        7 => Item::Sushi,
        8 => Item::Sandals,
        9 => Item::PC,
        _ => Item::Other,
    }
}

fn main() {
    // TODO: add signal handler for SIGINT and close threads properly
    println!("Warehouse schema");
    println!("\t0 Empty slot");
    println!("\t* Taken slot");
    println!("\t+ Recently taken by worker\n");

    let mut handles = Vec::with_capacity(NO_TH);
    let buffer = Arc::new(Mutex::new(Buffer::new(BUFFER_LEN)));
    let sem_full = Arc::new(Semaphore::new(0));
    let sem_empty = Arc::new(Semaphore::new(BUFFER_LEN as i32));
    let n_producers = 3;
    let n_consumers = 2;
    println!("Number of trucks  (producers): {}", n_producers);
    println!("Number of workers (consumers): {}", n_consumers);
    for i in 0..n_producers {
        let sem_full = sem_full.clone();
        let sem_empty = sem_empty.clone();
        let buffer = buffer.clone();
        handles.push(thread::spawn(move || {
            let mut item: Item;
            loop {
                sem_empty.wait();
                {
                    let buffer = buffer.lock().unwrap();
                    item = produce();
                    buffer.add(item);
                    println!(
                        "{}\t\tTruck [{}] added {} to warehouse",
                        buffer,
                        i + 1,
                        item
                    );
                    thread::sleep(time::Duration::from_secs(1));
                } // unlock
                sem_full.post();
            }
        }));
    }

    for i in 0..n_consumers {
        let sem_full = sem_full.clone();
        let sem_empty = sem_empty.clone();
        let buffer = buffer.clone();
        handles.push(thread::spawn(move || {
            let mut item: Item;
            loop {
                sem_full.wait();
                {
                    let buffer = buffer.lock().unwrap();
                    item = buffer.remove();
                    println!(
                        "{}\t\tWorker [{}] take {} from warehouse",
                        buffer,
                        i + 1,
                        item
                    );
                    consume(i + 1, item);
                    thread::sleep(time::Duration::from_secs(1));
                } // unlock
                sem_empty.post();
            }
        }));
    }

    for handle in handles {
        handle.join().unwrap();
    }
}
