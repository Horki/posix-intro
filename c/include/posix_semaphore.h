#ifndef POSIX_SEMAPHORE_H_
#define POSIX_SEMAPHORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

struct rk_sema {
#ifdef __APPLE__
  dispatch_semaphore_t sem;
#else
  sem_t sem;
#endif
};

static inline int rk_sema_init(struct rk_sema *s, int32_t shared,
                               uint32_t value) {
#ifdef __APPLE__
  dispatch_semaphore_t *sem = &s->sem;

  *sem = dispatch_semaphore_create(value);
  return 0;
#else
  return sem_init(&s->sem, shared, value);
#endif
}

static inline void rk_sema_wait(struct rk_sema *s) {
#ifdef __APPLE__
  dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
  int r;

  do {
    r = sem_wait(&s->sem);
  } while (r == -1 && errno == EINTR);
#endif
}

static inline void rk_sema_post(struct rk_sema *s) {
#ifdef __APPLE__
  dispatch_semaphore_signal(s->sem);
#else
  sem_post(&s->sem);
#endif
}

static inline int rk_sema_destroy(struct rk_sema *s) {
#ifdef __APPLE__
  return 0;
#else
  return sem_destroy(&s->sem);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
