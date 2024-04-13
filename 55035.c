int dump_fpu(struct pt_regs *regs, struct user_i387_struct *ufpu)
{
	struct task_struct *tsk = current;
	struct fpu *fpu = &tsk->thread.fpu;
	int fpvalid;

	fpvalid = fpu->fpstate_active;
	if (fpvalid)
		fpvalid = !fpregs_get(tsk, NULL,
				      0, sizeof(struct user_i387_ia32_struct),
				      ufpu, NULL);

	return fpvalid;
}
