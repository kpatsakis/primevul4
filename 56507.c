static bool access_vm_reg(struct kvm_vcpu *vcpu,
			  struct sys_reg_params *p,
			  const struct sys_reg_desc *r)
{
	bool was_enabled = vcpu_has_cache_enabled(vcpu);

	BUG_ON(!p->is_write);

	if (!p->is_aarch32) {
		vcpu_sys_reg(vcpu, r->reg) = p->regval;
	} else {
		if (!p->is_32bit)
			vcpu_cp15_64_high(vcpu, r->reg) = upper_32_bits(p->regval);
		vcpu_cp15_64_low(vcpu, r->reg) = lower_32_bits(p->regval);
	}

	kvm_toggle_cache(vcpu, was_enabled);
	return true;
}
