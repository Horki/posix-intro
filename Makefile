CC=clang
C_FLAGS=-std=c11 -Wall -Werror -g

RUSTC=rustc
RUSTC_FLAGS=-g --deny warnings

C_BINS=c_lab01

R_BINS=r_lab01

all: $(C_BINS) $(R_BINS)

%: %.c
	$(CC) $(C_FLAGS) -o $@ $^ -lpthread

%: %.rs
	rustup run stable $(RUSTC) $(RUSTC_FLAGS) -o $@ $^

clean:
	rm -f $(C_BINS) $(R_BINS) 
