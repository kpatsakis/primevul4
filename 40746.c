static struct perf_guest_switch_msr *intel_guest_get_msrs(int *nr)
{
	struct cpu_hw_events *cpuc = &__get_cpu_var(cpu_hw_events);
	struct perf_guest_switch_msr *arr = cpuc->guest_switch_msrs;

	arr[0].msr = MSR_CORE_PERF_GLOBAL_CTRL;
	arr[0].host = x86_pmu.intel_ctrl & ~cpuc->intel_ctrl_guest_mask;
	arr[0].guest = x86_pmu.intel_ctrl & ~cpuc->intel_ctrl_host_mask;
	/*
	 * If PMU counter has PEBS enabled it is not enough to disable counter
	 * on a guest entry since PEBS memory write can overshoot guest entry
	 * and corrupt guest memory. Disabling PEBS solves the problem.
	 */
	arr[1].msr = MSR_IA32_PEBS_ENABLE;
	arr[1].host = cpuc->pebs_enabled;
	arr[1].guest = 0;

	*nr = 2;
	return arr;
}
