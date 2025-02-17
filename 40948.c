void __perf_sw_event(u32 event_id, u64 nr, struct pt_regs *regs, u64 addr)
{
	struct perf_sample_data data;
	int rctx;

	preempt_disable_notrace();
	rctx = perf_swevent_get_recursion_context();
	if (rctx < 0)
		return;

	perf_sample_data_init(&data, addr, 0);

	do_perf_sw_event(PERF_TYPE_SOFTWARE, event_id, nr, &data, regs);

	perf_swevent_put_recursion_context(rctx);
	preempt_enable_notrace();
}
