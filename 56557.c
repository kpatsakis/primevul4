static bool trap_debug_regs(struct kvm_vcpu *vcpu,
			    struct sys_reg_params *p,
			    const struct sys_reg_desc *r)
{
	if (p->is_write) {
		vcpu_sys_reg(vcpu, r->reg) = p->regval;
		vcpu->arch.debug_flags |= KVM_ARM64_DEBUG_DIRTY;
	} else {
		p->regval = vcpu_sys_reg(vcpu, r->reg);
	}

	trace_trap_reg(__func__, r->reg, p->is_write, p->regval);

	return true;
}
