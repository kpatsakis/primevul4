int kvm_arm_sys_reg_get_reg(struct kvm_vcpu *vcpu, const struct kvm_one_reg *reg)
{
	const struct sys_reg_desc *r;
	void __user *uaddr = (void __user *)(unsigned long)reg->addr;

	if ((reg->id & KVM_REG_ARM_COPROC_MASK) == KVM_REG_ARM_DEMUX)
		return demux_c15_get(reg->id, uaddr);

	if (KVM_REG_SIZE(reg->id) != sizeof(__u64))
		return -ENOENT;

	r = index_to_sys_reg_desc(vcpu, reg->id);
	if (!r)
		return get_invariant_sys_reg(reg->id, uaddr);

	if (r->get_user)
		return (r->get_user)(vcpu, r, reg, uaddr);

	return reg_to_user(uaddr, &vcpu_sys_reg(vcpu, r->reg), reg->id);
}
