UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	OSX_BIN=*.dSYM
endif

CC=clang
C_FLAGS=-std=gnu11 -Wall -Werror -g

CPP=clang++
CPP_FLAGS=-std=c++17 -Wall -Werror -O3

RUSTC=rustc
RUSTC_FLAGS=-g --deny warnings

CPP_BINS=bin/cpp/cc_lab01 \
				 bin/cpp/cc_lab02_a bin/cpp/cc_lab02_b \
				 bin/cpp/cc_lab03_a bin/cpp/cc_lab03_b \
				 bin/cpp/cc_lab04 \
				 bin/cpp/cc_lab05_a bin/cpp/cc_lab05_b \
				 bin/cpp/cc_lab06

C_BINS=bin/c/c_lab01 \
			 bin/c/c_lab02_a bin/c/c_lab02_b \
			 bin/c/c_lab03_a bin/c/c_lab03_b \
			 bin/c/c_lab04 \
			 bin/c/c_lab05_a bin/c/c_lab05_b \
			 bin/c/c_lab06 \
			 bin/c/c_lab07_ipc_shm_w bin/c/c_lab07_ipc_shm_r \
			 bin/c/c_lab07_ipc_socket_server bin/c/c_lab07_ipc_socket_client

R_BINS=bin/rust/r_lab01 \
			 bin/rust/r_lab02_a bin/rust/r_lab02_b \
			 bin/rust/r_lab03_a bin/rust/r_lab03_b \
			 bin/rust/r_lab04 \
			 bin/rust/r_lab05_a bin/rust/r_lab05_b \
			 bin/rust/r_lab06

all: $(C_BINS) $(R_BINS) $(CPP_BINS)

bin/c/c_lab01: c/c_lab01.c
	mkdir -p bin/c && $(CC) $(C_FLAGS) -o $@ $^

bin/c/c_lab07_ipc_shm_w: c/c_lab07_ipc_shm_w.c c/include/types.h shared_object.txt
	mkdir -p bin/c && $(CC) $(C_FLAGS) -Ic/include -o $@ $< -lpthread

bin/c/c_lab07_ipc_shm_r: c/c_lab07_ipc_shm_r.c c/include/types.h shared_object.txt
	mkdir -p bin/c && $(CC) $(C_FLAGS) -Ic/include -o $@ $< -lpthread

bin/c/c_lab07_ipc_socket_server: c/ipc/message_passing/socket/server.c c/ipc/include/common.h
	mkdir -p bin/c && $(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $< -lpthread

bin/c/c_lab07_ipc_socket_client: c/ipc/message_passing/socket/client.c c/ipc/include/common.h
	mkdir -p bin/c && $(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $< -lpthread

shared_object.txt:
	echo "" > $@

bin/cpp/cc_lab01: cpp/cc_lab01.cc
	mkdir -p bin/cpp && $(CPP) $(CPP_FLAGS) -o $@ $^

bin/cpp/cc_lab05_b: cpp/cc_lab05_b.cc cpp/include/buffer.hh
	mkdir -p bin/cpp && $(CPP) $(CPP_FLAGS) -Icpp/include -o $@ $< -pthread

bin/cpp/%: cpp/%.cc
	mkdir -p bin/cpp && $(CPP) $(CPP_FLAGS) -Icpp/include -o $@ $^ -pthread

bin/c/%: c/%.c
	mkdir -p bin/c && $(CC) $(C_FLAGS) -Ic/include -o $@ $^ -lpthread

bin/rust/%: rust/%.rs
	mkdir -p bin/rust && rustup run stable $(RUSTC) $(RUSTC_FLAGS) -o $@ $^

clean:
	rm -rf bin/ *.txt $(OSX_BIN)
