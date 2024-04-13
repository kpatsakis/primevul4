int kill_pid_info_as_cred(int sig, struct siginfo *info, struct pid *pid,
			 const struct cred *cred, u32 secid)
{
	int ret = -EINVAL;
	struct task_struct *p;
	unsigned long flags;

	if (!valid_signal(sig))
		return ret;

	rcu_read_lock();
	p = pid_task(pid, PIDTYPE_PID);
	if (!p) {
		ret = -ESRCH;
		goto out_unlock;
	}
	if (si_fromuser(info) && !kill_as_cred_perm(cred, p)) {
		ret = -EPERM;
		goto out_unlock;
	}
	ret = security_task_kill(p, info, sig, secid);
	if (ret)
		goto out_unlock;

	if (sig) {
		if (lock_task_sighand(p, &flags)) {
			ret = __send_signal(sig, info, p, 1, 0);
			unlock_task_sighand(p, &flags);
		} else
			ret = -ESRCH;
	}
out_unlock:
	rcu_read_unlock();
	return ret;
}
