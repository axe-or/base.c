#include "prelude.h"
#include <stdatomic.h>

#define SPINLOCK_LOCKED 1
#define SPINLOCK_UNLOCKED 0

// The zeroed state of a spinlock is unlocked, to be effective across threads
// it's important to keep the spinlock outside of the stack and never mark it as
// a thread_local struct.
typedef struct {
	atomic_int _state;
} Spinlock;

// Enter a busy wait loop until spinlock is acquired(locked)
void spinlock_acquire(Spinlock* l);

// Try to lock spinlock, if failed, just move on. Returns if lock was locked.
bool spinlock_try_acquire(Spinlock* l);

// Release(unlock) the spinlock
void spinlock_release(Spinlock* l);

#define spinlock_scoped(LockPtr, Scope) \
	do { spinlock_acquire(LockPtr); do { Scope } while(0); spinlock_release(LockPtr); } while(0)
