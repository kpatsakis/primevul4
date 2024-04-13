int ldsem_down_read_trylock(struct ld_semaphore *sem)
{
	long count = sem->count;

	while (count >= 0) {
		if (ldsem_cmpxchg(&count, count + LDSEM_READ_BIAS, sem)) {
			lockdep_acquire_read(sem, 0, 1, _RET_IP_);
			lock_stat(sem, acquired);
			return 1;
		}
	}
	return 0;
}
