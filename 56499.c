static bool access_gic_sre(struct kvm_vcpu *vcpu,
			   struct sys_reg_params *p,
			   const struct sys_reg_desc *r)
{
	if (p->is_write)
		return ignore_write(vcpu, p);

	p->regval = vcpu->arch.vgic_cpu.vgic_v3.vgic_sre;
	return true;
}
