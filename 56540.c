static bool pmu_access_event_counter_el0_disabled(struct kvm_vcpu *vcpu)
{
	u64 reg = vcpu_sys_reg(vcpu, PMUSERENR_EL0);

	return !((reg & (ARMV8_PMU_USERENR_ER | ARMV8_PMU_USERENR_EN))
		 || vcpu_mode_priv(vcpu));
}
