static void perf_sample_regs_user(struct perf_regs_user *regs_user,
				  struct pt_regs *regs)
{
	if (!user_mode(regs)) {
		if (current->mm)
			regs = task_pt_regs(current);
		else
			regs = NULL;
	}

	if (regs) {
		regs_user->regs = regs;
		regs_user->abi  = perf_reg_abi(current);
	}
}
