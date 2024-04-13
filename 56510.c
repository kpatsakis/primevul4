static void dbg_to_reg(struct kvm_vcpu *vcpu,
		       struct sys_reg_params *p,
		       u64 *dbg_reg)
{
	p->regval = *dbg_reg;
	if (p->is_32bit)
		p->regval &= 0xffffffffUL;
}
