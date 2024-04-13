static int check_kill_permission(int sig, struct siginfo *info,
				 struct task_struct *t)
{
	struct pid *sid;
	int error;

	if (!valid_signal(sig))
		return -EINVAL;

	if (!si_fromuser(info))
		return 0;

	error = audit_signal_info(sig, t); /* Let audit system see the signal */
	if (error)
		return error;

	if (!same_thread_group(current, t) &&
	    !kill_ok_by_cred(t)) {
		switch (sig) {
		case SIGCONT:
			sid = task_session(t);
			/*
			 * We don't return the error if sid == NULL. The
			 * task was unhashed, the caller must notice this.
			 */
			if (!sid || sid == task_session(current))
				break;
		default:
			return -EPERM;
		}
	}

	return security_task_kill(t, info, sig, 0);
}
