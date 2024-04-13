static void update_perf_cpu_limits(void)
{
	u64 tmp = perf_sample_period_ns;

	tmp *= sysctl_perf_cpu_time_max_percent;
	tmp = div_u64(tmp, 100);
	if (!tmp)
		tmp = 1;

	WRITE_ONCE(perf_sample_allowed_ns, tmp);
}
