static bool access_pmceid(struct kvm_vcpu *vcpu, struct sys_reg_params *p,
			  const struct sys_reg_desc *r)
{
	u64 pmceid;

	if (!kvm_arm_pmu_v3_ready(vcpu))
		return trap_raz_wi(vcpu, p, r);

	BUG_ON(p->is_write);

	if (pmu_access_el0_disabled(vcpu))
		return false;

	if (!(p->Op2 & 1))
		pmceid = read_sysreg(pmceid0_el0);
	else
		pmceid = read_sysreg(pmceid1_el0);

	p->regval = pmceid;

	return true;
}
