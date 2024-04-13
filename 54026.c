static inline int __ldsem_down_read_nested(struct ld_semaphore *sem,
					   int subclass, long timeout)
{
	long count;

	lockdep_acquire_read(sem, subclass, 0, _RET_IP_);

	count = ldsem_atomic_update(LDSEM_READ_BIAS, sem);
	if (count <= 0) {
		lock_stat(sem, contended);
		if (!down_read_failed(sem, count, timeout)) {
			lockdep_release(sem, 1, _RET_IP_);
			return 0;
		}
	}
	lock_stat(sem, acquired);
	return 1;
}
