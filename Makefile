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
			 bin/cpp/cc_lab06 \
			 bin/cpp/cc_lab07_ipc_pipe \
			 bin/cpp/cc_lab07_ipc_mq_write bin/cpp/cc_lab07_ipc_mq_read \
			 bin/cpp/cc_lab07_ipc_socket_server bin/cpp/cc_lab07_ipc_socket_client

C_BINS=bin/c/c_lab01 \
			 bin/c/c_lab02_a bin/c/c_lab02_b \
			 bin/c/c_lab03_a bin/c/c_lab03_b \
			 bin/c/c_lab04 \
			 bin/c/c_lab05_a bin/c/c_lab05_b \
			 bin/c/c_lab06 \
			 bin/c/c_lab07_ipc_shm_w bin/c/c_lab07_ipc_shm_r \
			 bin/c/c_lab07_ipc_socket_server bin/c/c_lab07_ipc_socket_client \
			 bin/c/c_lab07_ipc_mq_write bin/c/c_lab07_ipc_mq_read \
			 bin/c/c_lab07_ipc_pipe \
			 bin/c/c_lab07_ipc_shm_w bin/c/c_lab07_ipc_shm_r

R_BINS=bin/rust/r_lab01 \
			 bin/rust/r_lab02_a bin/rust/r_lab02_b \
			 bin/rust/r_lab03_a bin/rust/r_lab03_b \
			 bin/rust/r_lab04 \
			 bin/rust/r_lab05_a bin/rust/r_lab05_b \
			 bin/rust/r_lab06

all: dirs $(C_BINS) $(R_BINS) $(CPP_BINS)

dirs:
	mkdir -p bin/{c,cpp,rust}

bin/c/c_lab01: c/c_lab01.c
	$(CC) $(C_FLAGS) -o $@ $^

bin/c/c_lab05_a: c/c_lab05_a.c c/include/posix_semaphore.h
	$(CC) $(C_FLAGS) -Ic/include -o $@ $< -lpthread

bin/c/c_lab07_ipc_shm_w: c/ipc/memory_based/shared_memory/write.c c/ipc/include/shm_type.h shared_object.txt
	$(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $< -lpthread

bin/c/c_lab07_ipc_shm_r: c/ipc/memory_based/shared_memory/read.c c/ipc/include/shm_type.h shared_object.txt
	$(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $< -lpthread

bin/c/c_lab07_ipc_socket_server: c/ipc/message_passing/socket/server.c c/ipc/include/socket.h
	$(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $<

bin/c/c_lab07_ipc_socket_client: c/ipc/message_passing/socket/client.c c/ipc/include/socket.h
	$(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $<

bin/c/c_lab07_ipc_mq_write: c/ipc/message_passing/message_queue/write.c c/ipc/include/type.h
	$(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $<

bin/c/c_lab07_ipc_mq_read: c/ipc/message_passing/message_queue/read.c c/ipc/include/type.h
	$(CC) $(C_FLAGS) -Ic/ipc/include -o $@ $<

bin/c/c_lab07_ipc_pipe: c/ipc/message_passing/pipe/twowaypipe.c
	$(CC) $(C_FLAGS) -o $@ $^

shared_object.txt:
	echo "" > $@

bin/cpp/cc_lab01: cpp/cc_lab01.cc cpp/include/custom_exception.hh
	$(CPP) $(CPP_FLAGS) -Icpp/include -o $@ $<

bin/cpp/cc_lab05_a: cpp/cc_lab05_a.cc cpp/include/buffer.hh c/include/posix_semaphore.h
	$(CPP) $(CPP_FLAGS) -Ic/include -Icpp/include -o $@ $< -pthread

bin/cpp/cc_lab05_b: cpp/cc_lab05_b.cc cpp/include/buffer.hh c/include/posix_semaphore.h
	$(CPP) $(CPP_FLAGS) -Ic/include -Icpp/include -o $@ $< -pthread

bin/cpp/cc_lab06: cpp/cc_lab06.cc cpp/include/common.hh c/include/posix_semaphore.h
	$(CPP) $(CPP_FLAGS) -Ic/include -Icpp/include -o $@ $< -pthread

bin/cpp/cc_lab07_ipc_pipe: cpp/ipc/message_passing/pipe/twowaypipe.cc
	$(CPP) $(CPP_FLAGS) -o $@ $<

bin/cpp/cc_lab07_ipc_mq_write: cpp/ipc/message_passing/message_queue/write.cc cpp/ipc/message_passing/message_queue/mqueue.hh
	$(CPP) $(CPP_FLAGS) -o $@ $<

bin/cpp/cc_lab07_ipc_mq_read: cpp/ipc/message_passing/message_queue/read.cc cpp/ipc/message_passing/message_queue/mqueue.hh
	$(CPP) $(CPP_FLAGS) -o $@ $<

bin/cpp/cc_lab07_ipc_socket_server: cpp/ipc/message_passing/socket/server.cc cpp/ipc/message_passing/socket/sockets.hh
	$(CPP) $(CPP_FLAGS) -o $@ $<

bin/cpp/cc_lab07_ipc_socket_client: cpp/ipc/message_passing/socket/client.cc cpp/ipc/message_passing/socket/sockets.hh
	$(CPP) $(CPP_FLAGS) -o $@ $<

bin/cpp/%: cpp/%.cc
	$(CPP) $(CPP_FLAGS) -Icpp/include -o $@ $^ -pthread

bin/c/%: c/%.c
	$(CC) $(C_FLAGS) -Ic/include -o $@ $^ -lpthread

bin/rust/%: rust/%.rs
	rustup run stable $(RUSTC) $(RUSTC_FLAGS) -o $@ $^

format:
	rustfmt rust/*.rs && clang-format cpp/*.cc cpp/ipc/*/*/*.cc cpp/ipc/*/*/*.hh cpp/include/*.hh c/*.c \
		c/include/*.h c/ipc/include/*.h \
		c/ipc/*/*/*.c -i

clean:
	rm -rf bin/ *.txt $(OSX_BIN)
