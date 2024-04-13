COMPAT_SYSCALL_DEFINE2(rt_sigpending, compat_sigset_t __user *, uset,
		compat_size_t, sigsetsize)
{
#ifdef __BIG_ENDIAN
	sigset_t set;
	int err = do_sigpending(&set, sigsetsize);
	if (!err) {
		compat_sigset_t set32;
		sigset_to_compat(&set32, &set);
		/* we can get here only if sigsetsize <= sizeof(set) */
		if (copy_to_user(uset, &set32, sigsetsize))
			err = -EFAULT;
	}
	return err;
#else
	return sys_rt_sigpending((sigset_t __user *)uset, sigsetsize);
#endif
}
