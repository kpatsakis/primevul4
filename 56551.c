static void reset_wvr(struct kvm_vcpu *vcpu,
		      const struct sys_reg_desc *rd)
{
	vcpu->arch.vcpu_debug_state.dbg_wvr[rd->reg] = rd->val;
}
