u64 scheduler_tick_max_deferment(void)
{
	struct rq *rq = this_rq();
	unsigned long next, now = ACCESS_ONCE(jiffies);

	next = rq->last_sched_tick + HZ;

	if (time_before_eq(next, now))
		return 0;

	return jiffies_to_nsecs(next - now);
}
