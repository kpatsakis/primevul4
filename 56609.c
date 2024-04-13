static void nested_ept_inject_page_fault(struct kvm_vcpu *vcpu,
		struct x86_exception *fault)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 exit_reason;
	unsigned long exit_qualification = vcpu->arch.exit_qualification;

	if (vmx->nested.pml_full) {
		exit_reason = EXIT_REASON_PML_FULL;
		vmx->nested.pml_full = false;
		exit_qualification &= INTR_INFO_UNBLOCK_NMI;
	} else if (fault->error_code & PFERR_RSVD_MASK)
		exit_reason = EXIT_REASON_EPT_MISCONFIG;
	else
		exit_reason = EXIT_REASON_EPT_VIOLATION;

	nested_vmx_vmexit(vcpu, exit_reason, 0, exit_qualification);
	vmcs12->guest_physical_address = fault->address;
}
