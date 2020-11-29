use std::cell::Cell;
use std::thread;

struct DataS {
    nums: (i32, i32, i32),
    // Cell; "interior mutability"
    total: Cell<i32>,
}

fn main() {
    let data_s = DataS {
        nums: (10, 10, 10),
        total: Cell::new(0),
    };
    let handle = thread::spawn(move || {
        for i in 1..=data_s.nums.0 {
            for j in 1..=data_s.nums.1 {
                for k in 1..=data_s.nums.2 {
                    // Cell update() is unstable feature
                    // data_s.total.update(|x| x + (i * j * k));
                    data_s.total.set(data_s.total.get() + (i * j * k));
                }
            }
        }
        // println!("sleeping");
        // thread::sleep(std::time::Duration::from_secs(1));
        println!("Thread finished");
        println!("Result = {}", data_s.total.get());
    });
    println!("Waiting thread to finish");
    handle.join().unwrap();
}
