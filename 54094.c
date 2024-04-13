SYSCALL_DEFINE3(sched_getattr, pid_t, pid, struct sched_attr __user *, uattr,
		unsigned int, size)
{
	struct sched_attr attr = {
		.size = sizeof(struct sched_attr),
	};
	struct task_struct *p;
	int retval;

	if (!uattr || pid < 0 || size > PAGE_SIZE ||
	    size < SCHED_ATTR_SIZE_VER0)
		return -EINVAL;

	rcu_read_lock();
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;

	retval = security_task_getscheduler(p);
	if (retval)
		goto out_unlock;

	attr.sched_policy = p->policy;
	if (p->sched_reset_on_fork)
		attr.sched_flags |= SCHED_FLAG_RESET_ON_FORK;
	if (task_has_dl_policy(p))
		__getparam_dl(p, &attr);
	else if (task_has_rt_policy(p))
		attr.sched_priority = p->rt_priority;
	else
		attr.sched_nice = TASK_NICE(p);

	rcu_read_unlock();

	retval = sched_read_attr(uattr, &attr, size);
	return retval;

out_unlock:
	rcu_read_unlock();
	return retval;
}
