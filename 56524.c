unsigned long kvm_arm_num_sys_reg_descs(struct kvm_vcpu *vcpu)
{
	return ARRAY_SIZE(invariant_sys_regs)
		+ num_demux_regs()
		+ walk_sys_regs(vcpu, (u64 __user *)NULL);
}
