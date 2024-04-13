int __sched ldsem_down_read(struct ld_semaphore *sem, long timeout)
{
	might_sleep();
	return __ldsem_down_read_nested(sem, 0, timeout);
}
