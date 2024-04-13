static bool access_pmuserenr(struct kvm_vcpu *vcpu, struct sys_reg_params *p,
			     const struct sys_reg_desc *r)
{
	if (!kvm_arm_pmu_v3_ready(vcpu))
		return trap_raz_wi(vcpu, p, r);

	if (p->is_write) {
		if (!vcpu_mode_priv(vcpu))
			return false;

		vcpu_sys_reg(vcpu, PMUSERENR_EL0) = p->regval
						    & ARMV8_PMU_USERENR_MASK;
	} else {
		p->regval = vcpu_sys_reg(vcpu, PMUSERENR_EL0)
			    & ARMV8_PMU_USERENR_MASK;
	}

	return true;
}
