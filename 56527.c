int kvm_handle_cp14_32(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	return kvm_handle_cp_32(vcpu,
				cp14_regs, ARRAY_SIZE(cp14_regs),
				NULL, 0);
}
