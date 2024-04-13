int fpregs_get(struct task_struct *target, const struct user_regset *regset,
	       unsigned int pos, unsigned int count,
	       void *kbuf, void __user *ubuf)
{
	struct fpu *fpu = &target->thread.fpu;
	struct user_i387_ia32_struct env;

	fpu__activate_fpstate_read(fpu);

	if (!boot_cpu_has(X86_FEATURE_FPU))
		return fpregs_soft_get(target, regset, pos, count, kbuf, ubuf);

	if (!boot_cpu_has(X86_FEATURE_FXSR))
		return user_regset_copyout(&pos, &count, &kbuf, &ubuf,
					   &fpu->state.fsave, 0,
					   -1);

	fpstate_sanitize_xstate(fpu);

	if (kbuf && pos == 0 && count == sizeof(env)) {
		convert_from_fxsr(kbuf, target);
		return 0;
	}

	convert_from_fxsr(&env, target);

	return user_regset_copyout(&pos, &count, &kbuf, &ubuf, &env, 0, -1);
}
