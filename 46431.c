static int sane_rt_signal_32_frame(unsigned int sp)
{
	struct rt_signal_frame_32 __user *sf;
	unsigned int regs;

	sf = (struct rt_signal_frame_32 __user *) (unsigned long) sp;
	if (read_user_stack_32((unsigned int __user *) &sf->uc.uc_regs, &regs))
		return 0;
	return regs == (unsigned long) &sf->uc.uc_mcontext;
}
