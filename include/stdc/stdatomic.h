/*
 * ISO C11: 7.17 Atomics <stdatomic.h>
 */

#ifndef _STDATOMIC_H
#define _STDATOMIC_H	1

/* Atomic types */
typedef _Atomic _Bool		atomic_bool;
typedef _Atomic char		atomic_char;
typedef _Atomic int		atomic_int;
typedef _Atomic long		atomic_long;
typedef _Atomic long long	atomic_llong;
typedef _Atomic unsigned char	atomic_uchar;
typedef _Atomic unsigned int	atomic_uint;
typedef _Atomic unsigned long	atomic_ulong;
typedef _Atomic unsigned long long atomic_ullong;
typedef _Atomic signed char		atomic_int8_t;
typedef _Atomic short int		atomic_int16_t;
typedef _Atomic int			atomic_int32_t;
typedef _Atomic long long int	atomic_int64_t;
typedef _Atomic unsigned char	atomic_uint8_t;
typedef _Atomic unsigned short int	atomic_uint16_t;
typedef _Atomic unsigned int		atomic_uint32_t;
typedef _Atomic unsigned long long int	atomic_uint64_t;
typedef _Atomic int			atomic_intptr_t;
typedef _Atomic unsigned int		atomic_uintptr_t;
typedef _Atomic unsigned int		atomic_size_t;

/* Memory order */
typedef enum {
	memory_order_relaxed,
	memory_order_consume,
	memory_order_acquire,
	memory_order_release,
	memory_order_acq_rel,
	memory_order_seq_cst
} memory_order;

/* Atomic operations as macros */
#define atomic_store(obj, val)			(*(obj) = (val))
#define atomic_load(obj)			(*(obj))
#define atomic_exchange(obj, val)		(*(obj) = (val))
#define atomic_fetch_add(obj, val)		(*(obj) += (val))
#define atomic_fetch_sub(obj, val)		(*(obj) -= (val))
#define atomic_fetch_and(obj, val)		(*(obj) &= (val))
#define atomic_fetch_or(obj, val)		(*(obj) |= (val))
#define atomic_fetch_xor(obj, val)		(*(obj) ^= (val))
#define atomic_is_lock_free(obj)		(1)
#define atomic_init(obj, val)			(*(obj) = (val))

#endif /* stdatomic.h */
