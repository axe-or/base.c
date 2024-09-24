#include "prelude.h"
#include <stdatomic.h>

typedef struct {
    _Atomic int state;
} Spinlock;

// Acquire (lock) the spinlock
void spinlock_acquire(Spinlock* l);

// Release (unlock) the spinlock
void spinlock_release(Spinlock* l);

#ifdef LIBC2_IMPLEMENTATION

void spinlock_acquire(Spinlock* l){
    for(;;){
        if(!atomic_exchange_explicit(&l->state, 1, memory_order_acquire)){
            break;
        }
        /* Busy wait */ while(atomic_load_explicit(&l->state, memory_order_relaxed));
    }
}

void spinlock_release(Spinlock* l){
    atomic_store_explicit(&l->state, 0, memory_order_release);
}

#endif