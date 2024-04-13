static int emulate_sys_reg(struct kvm_vcpu *vcpu,
			   struct sys_reg_params *params)
{
	size_t num;
	const struct sys_reg_desc *table, *r;

	table = get_target_table(vcpu->arch.target, true, &num);

	/* Search target-specific then generic table. */
	r = find_reg(params, table, num);
	if (!r)
		r = find_reg(params, sys_reg_descs, ARRAY_SIZE(sys_reg_descs));

	if (likely(r)) {
		/*
		 * Not having an accessor means that we have
		 * configured a trap that we don't know how to
		 * handle. This certainly qualifies as a gross bug
		 * that should be fixed right away.
		 */
		BUG_ON(!r->access);

		if (likely(r->access(vcpu, params, r))) {
			/* Skip instruction, since it was emulated */
			kvm_skip_instr(vcpu, kvm_vcpu_trap_il_is32bit(vcpu));
			return 1;
		}
		/* If access function fails, it should complain. */
	} else {
		kvm_err("Unsupported guest sys_reg access at: %lx\n",
			*vcpu_pc(vcpu));
		print_sys_reg_instr(params);
	}
	kvm_inject_undefined(vcpu);
	return 1;
}
