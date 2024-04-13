int __sched ldsem_down_write(struct ld_semaphore *sem, long timeout)
{
	might_sleep();
	return __ldsem_down_write_nested(sem, 0, timeout);
}
