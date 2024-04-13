static void unaccount_pmu_sb_event(struct perf_event *event)
{
	if (is_sb_event(event))
		detach_sb_event(event);
}
