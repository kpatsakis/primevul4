static bool pmu_counter_idx_valid(struct kvm_vcpu *vcpu, u64 idx)
{
	u64 pmcr, val;

	pmcr = vcpu_sys_reg(vcpu, PMCR_EL0);
	val = (pmcr >> ARMV8_PMU_PMCR_N_SHIFT) & ARMV8_PMU_PMCR_N_MASK;
	if (idx >= val && idx != ARMV8_PMU_CYCLE_IDX)
		return false;

	return true;
}
