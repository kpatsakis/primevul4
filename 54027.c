static inline int __ldsem_down_write_nested(struct ld_semaphore *sem,
					    int subclass, long timeout)
{
	long count;

	lockdep_acquire(sem, subclass, 0, _RET_IP_);

	count = ldsem_atomic_update(LDSEM_WRITE_BIAS, sem);
	if ((count & LDSEM_ACTIVE_MASK) != LDSEM_ACTIVE_BIAS) {
		lock_stat(sem, contended);
		if (!down_write_failed(sem, count, timeout)) {
			lockdep_release(sem, 1, _RET_IP_);
			return 0;
		}
	}
	lock_stat(sem, acquired);
	return 1;
}
