static void sched_dl_do_global(void)
{
	u64 new_bw = -1;
	int cpu;
	unsigned long flags;

	def_dl_bandwidth.dl_period = global_rt_period();
	def_dl_bandwidth.dl_runtime = global_rt_runtime();

	if (global_rt_runtime() != RUNTIME_INF)
		new_bw = to_ratio(global_rt_period(), global_rt_runtime());

	/*
	 * FIXME: As above...
	 */
	for_each_possible_cpu(cpu) {
		struct dl_bw *dl_b = dl_bw_of(cpu);

		raw_spin_lock_irqsave(&dl_b->lock, flags);
		dl_b->bw = new_bw;
		raw_spin_unlock_irqrestore(&dl_b->lock, flags);
	}
}
