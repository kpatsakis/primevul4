static inline void schedule_debug(struct task_struct *prev)
{
	/*
	 * Test if we are atomic. Since do_exit() needs to call into
	 * schedule() atomically, we ignore that path. Otherwise whine
	 * if we are scheduling when we should not.
	 */
	if (unlikely(in_atomic_preempt_off() && prev->state != TASK_DEAD))
		__schedule_bug(prev);
	rcu_sleep_check();

	profile_hit(SCHED_PROFILING, __builtin_return_address(0));

	schedstat_inc(this_rq(), sched_count);
}
