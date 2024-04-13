int kernel_execve(const char *filename,
		  const char *const argv[],
		  const char *const envp[])
{
	int ret = do_execve(filename,
			(const char __user *const __user *)argv,
			(const char __user *const __user *)envp);
	if (ret < 0)
		return ret;

	/*
	 * We were successful.  We won't be returning to our caller, but
	 * instead to user space by manipulating the kernel stack.
	 */
	ret_from_kernel_execve(current_pt_regs());
}
