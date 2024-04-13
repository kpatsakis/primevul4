static bool trap_oslsr_el1(struct kvm_vcpu *vcpu,
			   struct sys_reg_params *p,
			   const struct sys_reg_desc *r)
{
	if (p->is_write) {
		return ignore_write(vcpu, p);
	} else {
		p->regval = (1 << 3);
		return true;
	}
}
