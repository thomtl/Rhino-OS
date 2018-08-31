#pragma once

typedef struct {
  uint16_t lock;
} mutex_t;

extern void acquire_spinlock(mutex_t* mutex);
extern int release_spinlock(mutex_t* mutex);
