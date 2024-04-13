static inline int writer_trylock(struct ld_semaphore *sem)
{
	/* only wake this writer if the active part of the count can be
	 * transitioned from 0 -> 1
	 */
	long count = ldsem_atomic_update(LDSEM_ACTIVE_BIAS, sem);
	do {
		if ((count & LDSEM_ACTIVE_MASK) == LDSEM_ACTIVE_BIAS)
			return 1;
		if (ldsem_cmpxchg(&count, count - LDSEM_ACTIVE_BIAS, sem))
			return 0;
	} while (1);
}
