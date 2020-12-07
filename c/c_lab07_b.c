#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>

#define NUM_ELEMS 10000

typedef struct my_heap {
  char *p;
  uint64_t counter;
  uint64_t maxSize;
} my_heap;

char *arr[NUM_ELEMS];

my_heap arr_heap[] = {
    {NULL, 0, 0x00000008}, {NULL, 0, 0x00000010}, {NULL, 0, 0x00000020},
    {NULL, 0, 0x00000040}, {NULL, 0, 0x00000080}, {NULL, 0, 0x00000100},
    {NULL, 0, 0x00000200}, {NULL, 0, 0x00000400}, {NULL, 0, 0x00000800},
    {NULL, 0, 0x00001000}, {NULL, 0, 0x00002000}, {NULL, 0, 0x00004000},
    {NULL, 0, 0x00008000}, {NULL, 0, 0x00010000}, {NULL, 0, 0x00020000},
    {NULL, 0, 0x00040000}, {NULL, 0, 0x00080000}, {NULL, 0, 0x00100000},
    {NULL, 0, 0x00200000}, {NULL, 0, 0x00400000}, {NULL, 0, 0x00800000},
    {NULL, 0, 0x01000000}, {NULL, 0, 0x02000000}, {NULL, 0, 0x04000000},
    {NULL, 0, 0x08000000}, {NULL, 0, 0x10000000}, {NULL, 0, 0x20000000},
    {NULL, 0, 0x40000000}, {NULL, 0, 0x7f000000},
};

static void test_malloc(void *(*f)(size_t));
static void test_free(void *(*m)(size_t), void (*f)(void *));
static void print_heap();
static void *my_malloc(size_t);
static my_heap *find_heap(size_t);

int main() {
  print_heap();
  test_malloc(malloc);
  test_free(malloc, free);

  test_malloc(my_malloc);
  test_free(my_malloc, free);
  print_heap();

  return EXIT_SUCCESS;
}

static void print_heap() {
  my_heap *p = arr_heap;
  size_t heap_len = sizeof(arr_heap) / sizeof(arr_heap[0]);
  puts("START PRINT HEAP****************************");
  for (size_t i = 0; i < heap_len; ++i, ++p) {
    printf(
        "Heap block idx[%02zu] size: %12ld, used: %5ld, pointer handle: %p\n",
        i + 1, p->maxSize, p->counter, p->p);
  }
  puts("END PRINT HEAP******************************");
}

static void *my_malloc(size_t size) {
  char *tmp = NULL;
  my_heap **current = NULL;
  my_heap *p = find_heap(size);

  if (p == NULL) {
    return NULL;
  }

  tmp = (char *)mmap(p->p, p->maxSize, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (tmp != NULL) {
    p->counter++;
    current = (my_heap **)tmp;
    *current = p;

    tmp += sizeof(my_heap *);
  }

  return (void *)tmp;
}

static my_heap *find_heap(size_t size) {
  my_heap *p = arr_heap;
  size_t heap_len = sizeof(arr_heap) / sizeof(arr_heap[0]);
  while (heap_len--) {
    if (size <= p->maxSize) {
      if (p->p == NULL) {
        p->p = (char *)mmap(NULL, p->maxSize, PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (p->p == NULL) {
          return NULL;
        }
      }
      return p;
    }
    ++p;
  }
  return NULL;
}

static void test_malloc(void *(*f)(size_t)) {
  printf("Allocating memory with %d elements...\n", NUM_ELEMS);
  for (int32_t i = 0; i < NUM_ELEMS; ++i) {
    size_t size = rand() % CHAR_MAX;
    arr[i] = (char *)f(size);
  }
  printf("Alloc done");
}

static void test_free(void *(*m)(size_t), void (*f)(void *)) {
  puts("Freeing memory");
  for (int32_t i = 0; i < 50; ++i) {
    // for (int32_t j = 0; j < NUM_ELEMS / 2; ++j) {
    // f(arr[j]);
    // }
    //
    // for (int32_t j = 0; j < NUM_ELEMS / 2; ++j) {
    // size_t size = rand() % CHAR_MAX;
    // arr[i] = (char *) m(size);
    // }
    // for (int32_t j = NUM_ELEMS / 2; j < NUM_ELEMS; ++j)
    // f(arr[j]);
    //
    // for (int32_t j = NUM_ELEMS / 2; j < NUM_ELEMS; ++j) {
    // size_t size = rand() % CHAR_MAX;
    // arr[i] = (char *) m(size);
    // }
  }
  for (int32_t i = 0; i < NUM_ELEMS; ++i) {
    f(arr[i]);
    // arr[i] = NULL;
  }
  puts("Freeing done!!!");
}
