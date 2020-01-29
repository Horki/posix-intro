#ifndef INIT_SEM_H_
#define INIT_SEM_H_

#ifdef __APPLE__
// OsX
#include <dispatch/dispatch.h>

typedef sem_t dispatch_semaphore_t;

// TODO: check on mac
int sem_post(sem_t * s) {
  return dispatch_semaphore_signal(s);
}

// TODO: check on mac
int sem_wait(sem_t * s) {
  return dispatch_semaphore_wait(s, DISPATCH_TIME_FOREVER);
}

// TODO: check on mac
int sem_destroy(sem_t * s) {
  return dispatch_release(s);
}

#elif __linux__
// Linux
#include <semaphore.h>
#endif

#endif
