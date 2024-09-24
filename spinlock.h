#include <stdatomic.h>

#define SPINLOCK_LOCKED 1
#define SPINLOCK_UNLOCKED 0

typedef struct {
	atomic_int _state;
} Spinlock;

void spinlock_acquire(Spinlock* l);

void spinlock_release(Spinlock* l);

#define SCOPED_SPINLOCK(LockPtr, Scope) \
	do { spinlock_acquire(LockPtr); do { Scope } while(0); spinlock_release(LockPtr); } while(0)


#ifdef BASE_C_IMPLEMENTATION

void spinlock_acquire(Spinlock* l){
	for(;;){
		if(!atomic_exchange_explicit(&l->_state, SPINLOCK_LOCKED, memory_order_acquire)){
			break;
		}
		/* Busy wait while locked */
		while(atomic_load_explicit(&l->_state, memory_order_relaxed));
	}
}

void spinlock_release(Spinlock* l){
	atomic_store(&l->_state, SPINLOCK_UNLOCKED);
}
#endif
