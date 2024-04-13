static bool exclusive_event_match(struct perf_event *e1, struct perf_event *e2)
{
	if ((e1->pmu == e2->pmu) &&
	    (e1->cpu == e2->cpu ||
	     e1->cpu == -1 ||
	     e2->cpu == -1))
		return true;
	return false;
}
