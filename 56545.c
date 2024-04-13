static void reset_amair_el1(struct kvm_vcpu *vcpu, const struct sys_reg_desc *r)
{
	vcpu_sys_reg(vcpu, AMAIR_EL1) = read_sysreg(amair_el1);
}
