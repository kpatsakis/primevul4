static inline bool intel_pmu_needs_lbr_smpl(struct perf_event *event)
{
	/* user explicitly requested branch sampling */
	if (has_branch_stack(event))
		return true;

	/* implicit branch sampling to correct PEBS skid */
	if (x86_pmu.intel_cap.pebs_trap && event->attr.precise_ip > 1)
		return true;

	return false;
}
