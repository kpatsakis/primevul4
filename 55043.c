int xstateregs_get(struct task_struct *target, const struct user_regset *regset,
		unsigned int pos, unsigned int count,
		void *kbuf, void __user *ubuf)
{
	struct fpu *fpu = &target->thread.fpu;
	struct xregs_state *xsave;
	int ret;

	if (!boot_cpu_has(X86_FEATURE_XSAVE))
		return -ENODEV;

	xsave = &fpu->state.xsave;

	fpu__activate_fpstate_read(fpu);

	if (using_compacted_format()) {
		if (kbuf)
			ret = copy_xstate_to_kernel(kbuf, xsave, pos, count);
		else
			ret = copy_xstate_to_user(ubuf, xsave, pos, count);
	} else {
		fpstate_sanitize_xstate(fpu);
		/*
		 * Copy the 48 bytes defined by the software into the xsave
		 * area in the thread struct, so that we can copy the whole
		 * area to user using one user_regset_copyout().
		 */
		memcpy(&xsave->i387.sw_reserved, xstate_fx_sw_bytes, sizeof(xstate_fx_sw_bytes));

		/*
		 * Copy the xstate memory layout.
		 */
		ret = user_regset_copyout(&pos, &count, &kbuf, &ubuf, xsave, 0, -1);
	}
	return ret;
}
