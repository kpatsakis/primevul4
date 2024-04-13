SYSCALL_DEFINE2(munlock, unsigned long, start, size_t, len)
{
	int ret;

	len = PAGE_ALIGN(len + (start & ~PAGE_MASK));
	start &= PAGE_MASK;

	down_write(&current->mm->mmap_sem);
	ret = do_mlock(start, len, 0);
	up_write(&current->mm->mmap_sem);

	return ret;
}
