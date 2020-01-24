use std::thread;

const N: usize = 15;
struct Matrices([[i32; N]; N], [[i32; N]; N]);
// Only for educational purpose!
static mut RES: [[i32; N]; N] = [[0; N]; N];

fn print_matrix(grid: &[[i32; N]; N]) {
    for (_, row) in grid.iter().enumerate() {
        for (_, col) in row.iter().enumerate() {
            print!("{} ", col);
        }
        println!();
    }
}

fn init() -> Matrices {
    let a = [[2000; N]; N];
    let mut b = [[0; N]; N];
    for i in 0..N {
        for j in 0..N {
            b[i][j] = (j as i32) + 1;
        }
    }
    Matrices(a, b)
}

fn main() {
    let Matrices(a, b) = init();
    println!("Matrix A:");
    print_matrix(&a);
    println!("Matrix B:");
    print_matrix(&b);
    let mut handles = Vec::with_capacity(N);
    for i in 0..N {
        handles.push(thread::spawn(move || {
            println!("Calculating in {}. thread!", i + 1);
            for j in 0..N {
                unsafe {
                    RES[i][j] = a[i][j] + b[i][j];
                }
            }
            println!("Calculated in {}. thread! DONE!", i + 1);
        }));
    }
    for handle in handles {
        handle.join().unwrap();
    }
    println!("Matrix A + B:");
    unsafe {
        print_matrix(&RES);
    }
}
