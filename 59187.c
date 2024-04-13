static inline int __pmu_filter_match(struct perf_event *event)
{
	struct pmu *pmu = event->pmu;
	return pmu->filter_match ? pmu->filter_match(event) : 1;
}
