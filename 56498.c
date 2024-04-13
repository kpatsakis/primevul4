static bool access_gic_sgi(struct kvm_vcpu *vcpu,
			   struct sys_reg_params *p,
			   const struct sys_reg_desc *r)
{
	if (!p->is_write)
		return read_from_write_only(vcpu, p);

	vgic_v3_dispatch_sgi(vcpu, p->regval);

	return true;
}
