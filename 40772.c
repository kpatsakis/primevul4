COMPAT_SYSCALL_DEFINE4(rt_sigprocmask, int, how, compat_sigset_t __user *, nset,
		compat_sigset_t __user *, oset, compat_size_t, sigsetsize)
{
#ifdef __BIG_ENDIAN
	sigset_t old_set = current->blocked;

	/* XXX: Don't preclude handling different sized sigset_t's.  */
	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (nset) {
		compat_sigset_t new32;
		sigset_t new_set;
		int error;
		if (copy_from_user(&new32, nset, sizeof(compat_sigset_t)))
			return -EFAULT;

		sigset_from_compat(&new_set, &new32);
		sigdelsetmask(&new_set, sigmask(SIGKILL)|sigmask(SIGSTOP));

		error = sigprocmask(how, &new_set, NULL);
		if (error)
			return error;
	}
	if (oset) {
		compat_sigset_t old32;
		sigset_to_compat(&old32, &old_set);
		if (copy_to_user(oset, &old32, sizeof(compat_sigset_t)))
			return -EFAULT;
	}
	return 0;
#else
	return sys_rt_sigprocmask(how, (sigset_t __user *)nset,
				  (sigset_t __user *)oset, sigsetsize);
#endif
}
