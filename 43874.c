static void nested_ept_inject_page_fault(struct kvm_vcpu *vcpu,
		struct x86_exception *fault)
{
	struct vmcs12 *vmcs12;
	nested_vmx_vmexit(vcpu);
	vmcs12 = get_vmcs12(vcpu);

	if (fault->error_code & PFERR_RSVD_MASK)
		vmcs12->vm_exit_reason = EXIT_REASON_EPT_MISCONFIG;
	else
		vmcs12->vm_exit_reason = EXIT_REASON_EPT_VIOLATION;
	vmcs12->exit_qualification = vcpu->arch.exit_qualification;
	vmcs12->guest_physical_address = fault->address;
}
