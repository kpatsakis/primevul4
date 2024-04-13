int regset_xregset_fpregs_active(struct task_struct *target, const struct user_regset *regset)
{
	struct fpu *target_fpu = &target->thread.fpu;

	if (boot_cpu_has(X86_FEATURE_FXSR) && target_fpu->fpstate_active)
		return regset->n;
	else
		return 0;
}
