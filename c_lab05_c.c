#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


// Signals
// static void sig_int(int);
// static int catch_signal(int, void (*)(int));

// Producer Consumer
#define BUFFER_LEN 10
static int32_t buffer[BUFFER_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static size_t n_producers, n_consumers;
static int32_t producer_idx_buff = 0;
static int32_t consumer_idx_buff = 0;
static int32_t count = 0;

static void* consumer(void *);
static void* producer(void *);
static int32_t produce();
static void print_buffer();
static void add_to_buffer(int32_t);
static int32_t remove_from_buffer();
static void consume(int32_t, int32_t);
static const char* print_item(int32_t);

// Threads, Semaphores
#define NO_TH 5
static pthread_mutex_t mut_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;
static pthread_t threads[NO_TH];
static void wait_threads();
// static void cancel_threads();
static void error_msg(const char*);

int main() {
  // TODO: add signal handler for SIGINT and close threads properly
  srand(time(NULL));
  // if (catch_signal(SIGINT, sig_int) == -1) {
    // fprintf(stderr, "Can't map the signal handler\n");
    // exit(2);
  // }
  puts("Warehouse schema");
  puts("\t0 Empty slot");
  puts("\t* Taken slot");
  puts("\t+ Recently taken by worker\n");

  int32_t thread_no[NO_TH];
  n_producers = 3;
  n_consumers = 2;
  printf("Number of trucks  (producers): %ld\n", n_producers);
  printf("Number of workers (consumers): %ld\n", n_consumers);


  for (size_t i = 0; i < n_producers; ++i) {
    thread_no[i] = i;
    if (pthread_create(&threads[i], NULL, producer, (void *) &thread_no[i]) == -1)
      error_msg("Can't create threads for producers (trucks)");
  }
  for (size_t i = 0; i < n_consumers; ++i) {
    thread_no[i] = i;
    if (pthread_create(&threads[i + n_producers], NULL, consumer, (void *) &thread_no[i]) == -1)
      error_msg("Can't create threads for consumers (workers)");
  }

  wait_threads();

  return EXIT_SUCCESS;
}

static void* producer(void *a) {
  const int32_t producer_id = (*(int32_t *) a) + 1;
  int32_t item;
  while (true) {
    item = produce(); // generate item to put in buffer
    pthread_mutex_lock(&mut_lock);
    while (count == BUFFER_LEN) {
      pthread_mutex_unlock(&mut_lock);
      pthread_cond_wait(&cond_full, &mut_lock);
      pthread_mutex_lock(&mut_lock);
    }

    add_to_buffer(item);
    print_buffer();
    printf("\t\tTruck [%d] added %s to warehouse\n",
        producer_id, print_item(item));
    // sleep(1);
    if (count == 1) pthread_cond_signal(&cond_empty);
    pthread_mutex_unlock(&mut_lock);
  }
  return NULL;
}

static void* consumer(void *a) {
  const int32_t consumer_id = (*(int32_t *) a) + 1;
  int32_t item;

  while (true) {
    pthread_mutex_lock(&mut_lock);
    while (count == 0) {
      pthread_mutex_unlock(&mut_lock);
      pthread_cond_wait(&cond_empty, &mut_lock);
      pthread_mutex_lock(&mut_lock); 
    }

    item = remove_from_buffer();
    print_buffer();
    printf("\t\tWorker [%d] take %s from warehouse\n",
        consumer_id, print_item(item));
    // sleep(1);
    if (count == (BUFFER_LEN)) pthread_cond_signal(&cond_full);
    pthread_mutex_unlock(&mut_lock);
    consume(consumer_id, item);
  }
  return NULL;
}

static void error_msg(const char* m) {
  fprintf(stderr, "%s: %s\n", m, strerror(errno));
  exit(EXIT_FAILURE);
}

static int32_t produce() {
  int32_t item = (rand() % 9) + 1;
  // printf("Produce = %s\n", print_item(item));
  return item;
}

static void consume(int32_t consumer_id, int32_t item) {
  fprintf(stderr, "Worker [%d] sending %s to costumer\n",
      consumer_id, print_item(item));
}

static void add_to_buffer(int32_t item) {
  buffer[producer_idx_buff] = item;
  // incr index to buffer
  ++count;
  producer_idx_buff = (producer_idx_buff + 1) % BUFFER_LEN;
}

// 
// Warehouse
//
// 0 -> empty space in warehouse
// * -> taken space in warehouse
// + -> recently consumed space in warehouse
static void print_buffer() {
  putchar('[');
  for (int32_t i = 0; i < BUFFER_LEN; ++i) {
    if (buffer[i] == 0) {
      putchar('0');
    } else if (buffer[i] == -1) {
      putchar('+');
      // flag as empty
      buffer[i] = 0;
    } else {
      putchar('*');
    }
  }
  printf("]");
}

static int32_t remove_from_buffer() {
  int32_t item = buffer[consumer_idx_buff];
  buffer[consumer_idx_buff] = -1;
  --count;
  consumer_idx_buff = (consumer_idx_buff + 1) % BUFFER_LEN;

  return item;
}

// static void sig_int(int signo) {
  // printf("Catch signal %d\n", signo);
  // // Threads
  // puts("Closing threads");
  // cancel_threads();
  // // Semaphores
  // puts("Closing semaphores");
  // close_semaphores();
  // puts("Done");
  // exit(EXIT_SUCCESS);
// }

// TODO: fix LEAK, when closing
/*  LEAK SUMMARY: */
/*       definitely lost: 0 bytes in 0 blocks */
/*       indirectly lost: 0 bytes in 0 blocks */
/*         possibly lost: 1,360 bytes in 5 blocks */
// static int catch_signal(int signo, void (*handler)(int)) {
  // struct sigaction action;
  // action.sa_handler = handler;
  // sigemptyset(&action.sa_mask);
  // action.sa_flags = 0;
  // return sigaction(signo, &action, NULL);
// }

static void wait_threads() {
  for (size_t i = 0; i < NO_TH; ++i)
    if (pthread_join(threads[i], NULL) == -1)
      error_msg("Unable to join producer threads");
}


// static void cancel_threads() {
  // for (size_t i = 0; i < NO_TH; ++i)
    // if (pthread_cancel(threads[i]) != 0)
      // error_msg("Unable to cancel producer threads");
// }

static const char* print_item(int32_t item) {
  // printf("item = %d\n", item);
  switch (item) {
    case 1:
      return "hat";
    case 2:
      return "phone";
    case 3:
      return "dress";
    case 4:
      return "pants";
    case 5:
      return "shoes";
    case 6:
      return "burrito";
    case 7:
      return "sushi";
    case 8:
      return "sandals";
    case 9:
      return "pc";
    default:
      return "other";
  }
}
