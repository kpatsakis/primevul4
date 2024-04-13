static void vmx_leave_nested(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu)) {
		to_vmx(vcpu)->nested.nested_run_pending = 0;
		nested_vmx_vmexit(vcpu, -1, 0, 0);
	}
	free_nested(to_vmx(vcpu));
}
