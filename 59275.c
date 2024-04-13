void perf_sample_event_took(u64 sample_len_ns)
{
	u64 max_len = READ_ONCE(perf_sample_allowed_ns);
	u64 running_len;
	u64 avg_len;
	u32 max;

	if (max_len == 0)
		return;

	/* Decay the counter by 1 average sample. */
	running_len = __this_cpu_read(running_sample_length);
	running_len -= running_len/NR_ACCUMULATED_SAMPLES;
	running_len += sample_len_ns;
	__this_cpu_write(running_sample_length, running_len);

	/*
	 * Note: this will be biased artifically low until we have
	 * seen NR_ACCUMULATED_SAMPLES. Doing it this way keeps us
	 * from having to maintain a count.
	 */
	avg_len = running_len/NR_ACCUMULATED_SAMPLES;
	if (avg_len <= max_len)
		return;

	__report_avg = avg_len;
	__report_allowed = max_len;

	/*
	 * Compute a throttle threshold 25% below the current duration.
	 */
	avg_len += avg_len / 4;
	max = (TICK_NSEC / 100) * sysctl_perf_cpu_time_max_percent;
	if (avg_len < max)
		max /= (u32)avg_len;
	else
		max = 1;

	WRITE_ONCE(perf_sample_allowed_ns, avg_len);
	WRITE_ONCE(max_samples_per_tick, max);

	sysctl_perf_event_sample_rate = max * HZ;
	perf_sample_period_ns = NSEC_PER_SEC / sysctl_perf_event_sample_rate;

	if (!irq_work_queue(&perf_duration_work)) {
		early_printk("perf: interrupt took too long (%lld > %lld), lowering "
			     "kernel.perf_event_max_sample_rate to %d\n",
			     __report_avg, __report_allowed,
			     sysctl_perf_event_sample_rate);
	}
}
