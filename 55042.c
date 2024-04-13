int xfpregs_set(struct task_struct *target, const struct user_regset *regset,
		unsigned int pos, unsigned int count,
		const void *kbuf, const void __user *ubuf)
{
	struct fpu *fpu = &target->thread.fpu;
	int ret;

	if (!boot_cpu_has(X86_FEATURE_FXSR))
		return -ENODEV;

	fpu__activate_fpstate_write(fpu);
	fpstate_sanitize_xstate(fpu);

	ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf,
				 &fpu->state.fxsave, 0, -1);

	/*
	 * mxcsr reserved bits must be masked to zero for security reasons.
	 */
	fpu->state.fxsave.mxcsr &= mxcsr_feature_mask;

	/*
	 * update the header bits in the xsave header, indicating the
	 * presence of FP and SSE state.
	 */
	if (boot_cpu_has(X86_FEATURE_XSAVE))
		fpu->state.xsave.header.xfeatures |= XFEATURE_MASK_FPSSE;

	return ret;
}
