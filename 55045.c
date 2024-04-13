static inline int copy_fpregs_to_sigframe(struct xregs_state __user *buf)
{
	int err;

	if (use_xsave())
		err = copy_xregs_to_user(buf);
	else if (use_fxsr())
		err = copy_fxregs_to_user((struct fxregs_state __user *) buf);
	else
		err = copy_fregs_to_user((struct fregs_state __user *) buf);

	if (unlikely(err) && __clear_user(buf, fpu_user_xstate_size))
		err = -EFAULT;
	return err;
}
