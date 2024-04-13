static int intel_pmu_hw_config(struct perf_event *event)
{
	int ret = x86_pmu_hw_config(event);

	if (ret)
		return ret;

	if (event->attr.precise_ip && x86_pmu.pebs_aliases)
		x86_pmu.pebs_aliases(event);

	if (intel_pmu_needs_lbr_smpl(event)) {
		ret = intel_pmu_setup_lbr_filter(event);
		if (ret)
			return ret;
	}

	if (event->attr.type != PERF_TYPE_RAW)
		return 0;

	if (!(event->attr.config & ARCH_PERFMON_EVENTSEL_ANY))
		return 0;

	if (x86_pmu.version < 3)
		return -EINVAL;

	if (perf_paranoid_cpu() && !capable(CAP_SYS_ADMIN))
		return -EACCES;

	event->hw.config |= ARCH_PERFMON_EVENTSEL_ANY;

	return 0;
}
