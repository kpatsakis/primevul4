int kvm_arm_copy_sys_reg_indices(struct kvm_vcpu *vcpu, u64 __user *uindices)
{
	unsigned int i;
	int err;

	/* Then give them all the invariant registers' indices. */
	for (i = 0; i < ARRAY_SIZE(invariant_sys_regs); i++) {
		if (put_user(sys_reg_to_index(&invariant_sys_regs[i]), uindices))
			return -EFAULT;
		uindices++;
	}

	err = walk_sys_regs(vcpu, uindices);
	if (err < 0)
		return err;
	uindices += err;

	return write_demux_regids(uindices);
}
