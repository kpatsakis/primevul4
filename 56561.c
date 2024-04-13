static bool trap_xvr(struct kvm_vcpu *vcpu,
		     struct sys_reg_params *p,
		     const struct sys_reg_desc *rd)
{
	u64 *dbg_reg = &vcpu->arch.vcpu_debug_state.dbg_bvr[rd->reg];

	if (p->is_write) {
		u64 val = *dbg_reg;

		val &= 0xffffffffUL;
		val |= p->regval << 32;
		*dbg_reg = val;

		vcpu->arch.debug_flags |= KVM_ARM64_DEBUG_DIRTY;
	} else {
		p->regval = *dbg_reg >> 32;
	}

	trace_trap_reg(__func__, rd->reg, p->is_write, *dbg_reg);

	return true;
}
