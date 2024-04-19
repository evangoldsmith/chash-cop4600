#ifndef __rwlock_h__
#define __rwlock_h__

#include <semaphore.h>


typedef struct _rwlock_t {
    sem_t writelock;
    sem_t lock;
    int readers;
} rwlock_t;

void rwlock_init(rwlock_t *lock);
void rwlock_acquire_readlock(rwlock_t *lock);
void rwlock_release_readlock(rwlock_t *lock);
void rwlock_acquire_writelock(rwlock_t *lock);
void rwlock_release_writelock(rwlock_t *lock);
void *reader(void *arg);
void *writer(void *arg);

#endif /* __rwlock_h__ */
