int send_sig_info(int sig, struct siginfo *info, struct task_struct *p)
{
	/*
	 * Make sure legacy kernel users don't send in bad values
	 * (normal paths check this in check_kill_permission).
	 */
	if (!valid_signal(sig))
		return -EINVAL;

	return do_send_sig_info(sig, info, p, false);
}
