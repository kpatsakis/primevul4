static void update_pmu_context(struct pmu *pmu, struct pmu *old_pmu)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct perf_cpu_context *cpuctx;

		cpuctx = per_cpu_ptr(pmu->pmu_cpu_context, cpu);

		if (cpuctx->unique_pmu == old_pmu)
			cpuctx->unique_pmu = pmu;
	}
}
