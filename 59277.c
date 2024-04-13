static void perf_sched_delayed(struct work_struct *work)
{
	mutex_lock(&perf_sched_mutex);
	if (atomic_dec_and_test(&perf_sched_count))
		static_branch_disable(&perf_sched_events);
	mutex_unlock(&perf_sched_mutex);
}
