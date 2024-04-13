SYSCALL_DEFINE2(rt_sigpending, sigset_t __user *, uset, size_t, sigsetsize)
{
	sigset_t set;
	int err = do_sigpending(&set, sigsetsize);
	if (!err && copy_to_user(uset, &set, sigsetsize))
		err = -EFAULT;
	return err;
}
