#include "./spinlock.h"

void spinlock_acquire(Spinlock* l){
	for(;;){
		if(!atomic_exchange_explicit(&l->_state, SPINLOCK_LOCKED, memory_order_acquire)){
			break;
		}
		/* Busy wait while locked */
		while(atomic_load_explicit(&l->_state, memory_order_relaxed));
	}
}

bool spinlock_try_acquire(Spinlock* l){
    return !atomic_exchange_explicit(&l->_state, SPINLOCK_LOCKED, memory_order_acquire);
}

void spinlock_release(Spinlock* l){
	atomic_store(&l->_state, SPINLOCK_UNLOCKED);
}
