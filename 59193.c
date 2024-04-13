static void account_pmu_sb_event(struct perf_event *event)
{
	if (is_sb_event(event))
		attach_sb_event(event);
}
