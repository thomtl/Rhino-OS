#pragma once

typedef struct {
  uint16_t lock;
} spinlock_mutex_t;

extern void acquire_spinlock(spinlock_mutex_t* mutex);
extern int release_spinlock(spinlock_mutex_t* mutex);
