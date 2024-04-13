void ldsem_up_write(struct ld_semaphore *sem)
{
	long count;

	lockdep_release(sem, 1, _RET_IP_);

	count = ldsem_atomic_update(-LDSEM_WRITE_BIAS, sem);
	if (count < 0)
		ldsem_wake(sem);
}
