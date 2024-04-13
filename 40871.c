static inline void userns_fixup_signal_uid(struct siginfo *info, struct task_struct *t)
{
	if (current_user_ns() == task_cred_xxx(t, user_ns))
		return;

	if (SI_FROMKERNEL(info))
		return;

	rcu_read_lock();
	info->si_uid = from_kuid_munged(task_cred_xxx(t, user_ns),
					make_kuid(current_user_ns(), info->si_uid));
	rcu_read_unlock();
}
