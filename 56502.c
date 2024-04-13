static bool access_pmovs(struct kvm_vcpu *vcpu, struct sys_reg_params *p,
			 const struct sys_reg_desc *r)
{
	u64 mask = kvm_pmu_valid_counter_mask(vcpu);

	if (!kvm_arm_pmu_v3_ready(vcpu))
		return trap_raz_wi(vcpu, p, r);

	if (pmu_access_el0_disabled(vcpu))
		return false;

	if (p->is_write) {
		if (r->CRm & 0x2)
			/* accessing PMOVSSET_EL0 */
			kvm_pmu_overflow_set(vcpu, p->regval & mask);
		else
			/* accessing PMOVSCLR_EL0 */
			vcpu_sys_reg(vcpu, PMOVSSET_EL0) &= ~(p->regval & mask);
	} else {
		p->regval = vcpu_sys_reg(vcpu, PMOVSSET_EL0) & mask;
	}

	return true;
}
