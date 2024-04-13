void perf_sched_cb_inc(struct pmu *pmu)
{
	struct perf_cpu_context *cpuctx = this_cpu_ptr(pmu->pmu_cpu_context);

	if (!cpuctx->sched_cb_usage++)
		list_add(&cpuctx->sched_cb_entry, this_cpu_ptr(&sched_cb_list));

	this_cpu_inc(perf_sched_cb_usages);
}
