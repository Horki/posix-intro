CC=clang
C_FLAGS=-std=gnu11 -Wall -Werror -g

CPP=clang++
CPP_FLAGS=-std=c++17 -Wall -Werror -O3

RUSTC=rustc
RUSTC_FLAGS=-g --deny warnings

CPP_BINS=cc_lab01 \
				 cc_lab02_a cc_lab02_b \
				 cc_lab03_a cc_lab03_b

C_BINS=c_lab01 c_lab02_a c_lab02_b c_lab03_a c_lab03_b c_lab04 \
			 c_lab05_a c_lab05_b c_lab05_c c_lab05_d \
			 c_lab06 \
			 c_lab07_w c_lab07_r

R_BINS=r_lab01 r_lab02_a r_lab02_b r_lab03_a r_lab03_b r_lab04 \
			 r_lab05_a r_lab05_b r_lab06

all: $(C_BINS) $(R_BINS) $(CPP_BINS)

c_lab01: c_lab01.c
	$(CC) $(C_FLAGS) -o $@ $^

c_lab07_w: c_lab07_w.c include/types.h shared_object.txt
	$(CC) $(C_FLAGS) -Iinclude -o $@ $< -lpthread

c_lab07_r: c_lab07_r.c include/types.h shared_object.txt
	$(CC) $(C_FLAGS) -Iinclude -o $@ $< -lpthread

shared_object.txt:
	echo "" > $@

cc_lab01: cc_lab01.cc
	$(CPP) $(CPP_FLAGS) -o $@ $^

%: %.cc
	$(CPP) $(CPP_FLAGS) -o $@ $^ -pthread

%: %.c
	$(CC) $(C_FLAGS) -o $@ $^ -lpthread

%: %.rs
	rustup run stable $(RUSTC) $(RUSTC_FLAGS) -o $@ $^

clean:
	rm -f $(C_BINS) $(R_BINS) $(CPP_BINS) *.txt
