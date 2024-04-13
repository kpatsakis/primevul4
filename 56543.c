static void reg_to_dbg(struct kvm_vcpu *vcpu,
		       struct sys_reg_params *p,
		       u64 *dbg_reg)
{
	u64 val = p->regval;

	if (p->is_32bit) {
		val &= 0xffffffffUL;
		val |= ((*dbg_reg >> 32) << 32);
	}

	*dbg_reg = val;
	vcpu->arch.debug_flags |= KVM_ARM64_DEBUG_DIRTY;
}
