static void intel_pmu_disable_event(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	struct cpu_hw_events *cpuc = &__get_cpu_var(cpu_hw_events);

	if (unlikely(hwc->idx == INTEL_PMC_IDX_FIXED_BTS)) {
		intel_pmu_disable_bts();
		intel_pmu_drain_bts_buffer();
		return;
	}

	cpuc->intel_ctrl_guest_mask &= ~(1ull << hwc->idx);
	cpuc->intel_ctrl_host_mask &= ~(1ull << hwc->idx);

	/*
	 * must disable before any actual event
	 * because any event may be combined with LBR
	 */
	if (intel_pmu_needs_lbr_smpl(event))
		intel_pmu_lbr_disable(event);

	if (unlikely(hwc->config_base == MSR_ARCH_PERFMON_FIXED_CTR_CTRL)) {
		intel_pmu_disable_fixed(hwc);
		return;
	}

	x86_pmu_disable_event(event);

	if (unlikely(event->attr.precise_ip))
		intel_pmu_pebs_disable(event);
}
