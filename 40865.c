specific_send_sig_info(int sig, struct siginfo *info, struct task_struct *t)
{
	return send_signal(sig, info, t, 0);
}
