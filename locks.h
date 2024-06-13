#define LOCK_0 ((spin_lock_t * ) 0xd0000140)
#define LOCK_1 ((spin_lock_t * ) 0xd0000144)
#define LOCK_2 ((spin_lock_t * ) 0xd0000148)
#define LOCK_3 ((spin_lock_t * ) 0xd000014c)
#define FRAMEWORK
#ifdef FRAMEWORK

#define __LOCK_FLAG__ (irq = spin_lock_blocking(LOCK_0));
#define __UNLOCK_FLAG__ spin_unlock(LOCK_0, irq);
#define __LOCK_MEM__ spin_lock_unsafe_blocking(LOCK_1);
#define __UNLOCK_MEM__ spin_unlock_unsafe(LOCK_1);

#endif

#ifndef FRAMEWORK

#define __LOCK_FLAG__
#define __UNLOCK_FLAG__
#define __LOCK_MEM__
#define __UNLOCK_MEM__

#endif