static bool trap_dbgauthstatus_el1(struct kvm_vcpu *vcpu,
				   struct sys_reg_params *p,
				   const struct sys_reg_desc *r)
{
	if (p->is_write) {
		return ignore_write(vcpu, p);
	} else {
		p->regval = read_sysreg(dbgauthstatus_el1);
		return true;
	}
}
