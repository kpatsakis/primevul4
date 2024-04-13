int ldsem_down_write_nested(struct ld_semaphore *sem, int subclass,
			    long timeout)
{
	might_sleep();
	return __ldsem_down_write_nested(sem, subclass, timeout);
}
