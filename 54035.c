int ldsem_down_write_trylock(struct ld_semaphore *sem)
{
	long count = sem->count;

	while ((count & LDSEM_ACTIVE_MASK) == 0) {
		if (ldsem_cmpxchg(&count, count + LDSEM_WRITE_BIAS, sem)) {
			lockdep_acquire(sem, 0, 1, _RET_IP_);
			lock_stat(sem, acquired);
			return 1;
		}
	}
	return 0;
}
