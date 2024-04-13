static int intel_pmu_cpu_prepare(int cpu)
{
	struct cpu_hw_events *cpuc = &per_cpu(cpu_hw_events, cpu);

	if (!(x86_pmu.extra_regs || x86_pmu.lbr_sel_map))
		return NOTIFY_OK;

	cpuc->shared_regs = allocate_shared_regs(cpu);
	if (!cpuc->shared_regs)
		return NOTIFY_BAD;

	return NOTIFY_OK;
}
