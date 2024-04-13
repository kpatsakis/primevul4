COMPAT_SYSCALL_DEFINE4(rt_sigaction, int, sig,
		const struct compat_sigaction __user *, act,
		struct compat_sigaction __user *, oact,
		compat_size_t, sigsetsize)
{
	struct k_sigaction new_ka, old_ka;
	compat_sigset_t mask;
#ifdef __ARCH_HAS_SA_RESTORER
	compat_uptr_t restorer;
#endif
	int ret;

	/* XXX: Don't preclude handling different sized sigset_t's.  */
	if (sigsetsize != sizeof(compat_sigset_t))
		return -EINVAL;

	if (act) {
		compat_uptr_t handler;
		ret = get_user(handler, &act->sa_handler);
		new_ka.sa.sa_handler = compat_ptr(handler);
#ifdef __ARCH_HAS_SA_RESTORER
		ret |= get_user(restorer, &act->sa_restorer);
		new_ka.sa.sa_restorer = compat_ptr(restorer);
#endif
		ret |= copy_from_user(&mask, &act->sa_mask, sizeof(mask));
		ret |= __get_user(new_ka.sa.sa_flags, &act->sa_flags);
		if (ret)
			return -EFAULT;
		sigset_from_compat(&new_ka.sa.sa_mask, &mask);
	}

	ret = do_sigaction(sig, act ? &new_ka : NULL, oact ? &old_ka : NULL);
	if (!ret && oact) {
		sigset_to_compat(&mask, &old_ka.sa.sa_mask);
		ret = put_user(ptr_to_compat(old_ka.sa.sa_handler), 
			       &oact->sa_handler);
		ret |= copy_to_user(&oact->sa_mask, &mask, sizeof(mask));
		ret |= __put_user(old_ka.sa.sa_flags, &oact->sa_flags);
#ifdef __ARCH_HAS_SA_RESTORER
		ret |= put_user(ptr_to_compat(old_ka.sa.sa_restorer),
				&oact->sa_restorer);
#endif
	}
	return ret;
}
