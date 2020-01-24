#ifndef __SIMPLE_MONITOR_H_
#define __SIMPLE_MONITOR_H_

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_NUM 10

typedef struct monitor_tag {
  int32_t cnt;
  int32_t somebody_waits;
  pthread_mutex_t mut;
  pthread_cond_t cond;
} monitor_tag;

monitor_tag* monitor_init() {
  monitor_tag *m = (monitor_tag *) malloc(sizeof(monitor_tag));
  m->somebody_waits = 0;
  m->cnt = 0;
  // m->mut = PTHREAD_MUTEX_INITIALIZER;
  // m->cont = PTHREAD_COND_INITIALIZER;
  pthread_mutex_init(&m->mut, NULL);
  pthread_cond_init(&m->cond, NULL);

  return m;
}

void monitor_wait(monitor_tag* m) {
  pthread_mutex_lock(&m->mut);
  m->somebody_waits = 1;
  pthread_cond_wait(&m->cond, &m->mut);
  m->somebody_waits = 0;
  pthread_mutex_unlock(&m->mut);
}

void monitor_knock(monitor_tag* m) {
  pthread_mutex_lock(&m->mut);
  if (m->somebody_waits == 1) {
    m->cnt++;
    if (m->cnt == MAX_NUM) {
      pthread_cond_signal(&m->cond);
      m->cnt = 0;
    }
  }
  pthread_mutex_unlock(&m->mut);
}

void monitor_destroy(monitor_tag* m) {
  pthread_mutex_destroy(&m->mut);
  pthread_cond_destroy(&m->cond);
  free(m);
  m = NULL;
}


#endif
