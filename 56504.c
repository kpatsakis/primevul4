static bool access_pmswinc(struct kvm_vcpu *vcpu, struct sys_reg_params *p,
			   const struct sys_reg_desc *r)
{
	u64 mask;

	if (!kvm_arm_pmu_v3_ready(vcpu))
		return trap_raz_wi(vcpu, p, r);

	if (pmu_write_swinc_el0_disabled(vcpu))
		return false;

	if (p->is_write) {
		mask = kvm_pmu_valid_counter_mask(vcpu);
		kvm_pmu_software_increment(vcpu, p->regval & mask);
		return true;
	}

	return false;
}
