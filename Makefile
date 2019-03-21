CC=clang
C_FLAGS=-std=gnu11 -Wall -Werror -g

RUSTC=rustc
RUSTC_FLAGS=-g --deny warnings

C_BINS=c_lab01 c_lab02_a c_lab02_b c_lab03_a c_lab03_b c_lab04 \
			 c_lab05_a c_lab05_b c_lab06

R_BINS=r_lab01 r_lab02_a r_lab02_b r_lab03_a r_lab03_b r_lab04 \
			 r_lab05_a r_lab05_b r_lab06

all: $(C_BINS) $(R_BINS)

c_lab01: c_lab01.c
	$(CC) $(C_FLAGS) -o $@ $^

%: %.c
	$(CC) $(C_FLAGS) -o $@ $^ -lpthread

%: %.rs
	rustup run stable $(RUSTC) $(RUSTC_FLAGS) -o $@ $^

clean:
	rm -f $(C_BINS) $(R_BINS)
