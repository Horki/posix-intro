use std::process;

fn main() {
    let mut child = process::Command::new("gedit")
        .spawn()
        .unwrap_or_else(|errno| {
            eprintln!("Can't open gedit : {}", errno);
            process::exit(1);
        });

    println!(
        "I'm a parent process[ppid={}] waiting for a child",
        process::id()
    );
    println!("Waiting to close child process (gedit[pid={}])", child.id());
    let pid_status = child.wait().expect("Error waiting child process");
    println!("Child exited with {}", pid_status);
}
