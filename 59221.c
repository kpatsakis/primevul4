static void perf_duration_warn(struct irq_work *w)
{
	printk_ratelimited(KERN_INFO
		"perf: interrupt took too long (%lld > %lld), lowering "
		"kernel.perf_event_max_sample_rate to %d\n",
		__report_avg, __report_allowed,
		sysctl_perf_event_sample_rate);
}
