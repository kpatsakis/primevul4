COMPAT_SYSCALL_DEFINE2(rt_sigsuspend, compat_sigset_t __user *, unewset, compat_size_t, sigsetsize)
{
#ifdef __BIG_ENDIAN
	sigset_t newset;
	compat_sigset_t newset32;

	/* XXX: Don't preclude handling different sized sigset_t's.  */
	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (copy_from_user(&newset32, unewset, sizeof(compat_sigset_t)))
		return -EFAULT;
	sigset_from_compat(&newset, &newset32);
	return sigsuspend(&newset);
#else
	/* on little-endian bitmaps don't care about granularity */
	return sys_rt_sigsuspend((sigset_t __user *)unewset, sigsetsize);
#endif
}
