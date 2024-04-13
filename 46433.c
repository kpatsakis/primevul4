static int sane_signal_64_frame(unsigned long sp)
{
	struct signal_frame_64 __user *sf;
	unsigned long pinfo, puc;

	sf = (struct signal_frame_64 __user *) sp;
	if (read_user_stack_64((unsigned long __user *) &sf->pinfo, &pinfo) ||
	    read_user_stack_64((unsigned long __user *) &sf->puc, &puc))
		return 0;
	return pinfo == (unsigned long) &sf->info &&
		puc == (unsigned long) &sf->uc;
}
