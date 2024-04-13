static void intel_pmu_flush_branch_stack(void)
{
	/*
	 * Intel LBR does not tag entries with the
	 * PID of the current task, then we need to
	 * flush it on ctxsw
	 * For now, we simply reset it
	 */
	if (x86_pmu.lbr_nr)
		intel_pmu_lbr_reset();
}
