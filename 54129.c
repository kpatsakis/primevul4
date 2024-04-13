static int dl_overflow(struct task_struct *p, int policy,
		       const struct sched_attr *attr)
{

	struct dl_bw *dl_b = dl_bw_of(task_cpu(p));
	u64 period = attr->sched_period ?: attr->sched_deadline;
	u64 runtime = attr->sched_runtime;
	u64 new_bw = dl_policy(policy) ? to_ratio(period, runtime) : 0;
	int cpus, err = -1;

	if (new_bw == p->dl.dl_bw)
		return 0;

	/*
	 * Either if a task, enters, leave, or stays -deadline but changes
	 * its parameters, we may need to update accordingly the total
	 * allocated bandwidth of the container.
	 */
	raw_spin_lock(&dl_b->lock);
	cpus = dl_bw_cpus(task_cpu(p));
	if (dl_policy(policy) && !task_has_dl_policy(p) &&
	    !__dl_overflow(dl_b, cpus, 0, new_bw)) {
		__dl_add(dl_b, new_bw);
		err = 0;
	} else if (dl_policy(policy) && task_has_dl_policy(p) &&
		   !__dl_overflow(dl_b, cpus, p->dl.dl_bw, new_bw)) {
		__dl_clear(dl_b, p->dl.dl_bw);
		__dl_add(dl_b, new_bw);
		err = 0;
	} else if (!dl_policy(policy) && task_has_dl_policy(p)) {
		__dl_clear(dl_b, p->dl.dl_bw);
		err = 0;
	}
	raw_spin_unlock(&dl_b->lock);

	return err;
}
