int intel_pmu_save_and_restart(struct perf_event *event)
{
	x86_perf_event_update(event);
	return x86_perf_event_set_period(event);
}
