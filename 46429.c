static int read_user_stack_64(unsigned long __user *ptr, unsigned long *ret)
{
	if ((unsigned long)ptr > TASK_SIZE - sizeof(unsigned long) ||
	    ((unsigned long)ptr & 7))
		return -EFAULT;

	pagefault_disable();
	if (!__get_user_inatomic(*ret, ptr)) {
		pagefault_enable();
		return 0;
	}
	pagefault_enable();

	return read_user_stack_slow(ptr, ret, 8);
}
