int xfpregs_get(struct task_struct *target, const struct user_regset *regset,
		unsigned int pos, unsigned int count,
		void *kbuf, void __user *ubuf)
{
	struct fpu *fpu = &target->thread.fpu;

	if (!boot_cpu_has(X86_FEATURE_FXSR))
		return -ENODEV;

	fpu__activate_fpstate_read(fpu);
	fpstate_sanitize_xstate(fpu);

	return user_regset_copyout(&pos, &count, &kbuf, &ubuf,
				   &fpu->state.fxsave, 0, -1);
}
