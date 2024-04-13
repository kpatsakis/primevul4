static int read_user_stack_32(unsigned int __user *ptr, unsigned int *ret)
{
	if ((unsigned long)ptr > TASK_SIZE - sizeof(unsigned int) ||
	    ((unsigned long)ptr & 3))
		return -EFAULT;

	pagefault_disable();
	if (!__get_user_inatomic(*ret, ptr)) {
		pagefault_enable();
		return 0;
	}
	pagefault_enable();

	return read_user_stack_slow(ptr, ret, 4);
}
