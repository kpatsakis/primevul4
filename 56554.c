static int set_wvr(struct kvm_vcpu *vcpu, const struct sys_reg_desc *rd,
		const struct kvm_one_reg *reg, void __user *uaddr)
{
	__u64 *r = &vcpu->arch.vcpu_debug_state.dbg_wvr[rd->reg];

	if (copy_from_user(r, uaddr, KVM_REG_SIZE(reg->id)) != 0)
		return -EFAULT;
	return 0;
}
