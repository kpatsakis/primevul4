static bool access_pmselr(struct kvm_vcpu *vcpu, struct sys_reg_params *p,
			  const struct sys_reg_desc *r)
{
	if (!kvm_arm_pmu_v3_ready(vcpu))
		return trap_raz_wi(vcpu, p, r);

	if (pmu_access_event_counter_el0_disabled(vcpu))
		return false;

	if (p->is_write)
		vcpu_sys_reg(vcpu, PMSELR_EL0) = p->regval;
	else
		/* return PMSELR.SEL field */
		p->regval = vcpu_sys_reg(vcpu, PMSELR_EL0)
			    & ARMV8_PMU_COUNTER_MASK;

	return true;
}
