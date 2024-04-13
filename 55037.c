int regset_fpregs_active(struct task_struct *target, const struct user_regset *regset)
{
	struct fpu *target_fpu = &target->thread.fpu;

	return target_fpu->fpstate_active ? regset->n : 0;
}
