SYSCALL_DEFINE2(mlock, unsigned long, start, size_t, len)
{
	unsigned long locked;
	unsigned long lock_limit;
	int error = -ENOMEM;

	if (!can_do_mlock())
		return -EPERM;

	lru_add_drain_all();	/* flush pagevec */

	len = PAGE_ALIGN(len + (start & ~PAGE_MASK));
	start &= PAGE_MASK;

	lock_limit = rlimit(RLIMIT_MEMLOCK);
	lock_limit >>= PAGE_SHIFT;
	locked = len >> PAGE_SHIFT;

	down_write(&current->mm->mmap_sem);

	locked += current->mm->locked_vm;

	/* check against resource limits */
	if ((locked <= lock_limit) || capable(CAP_IPC_LOCK))
		error = do_mlock(start, len, 1);

	up_write(&current->mm->mmap_sem);
	if (!error)
		error = __mm_populate(start, len, 0);
	return error;
}
