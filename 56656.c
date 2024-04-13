static void vmx_inject_page_fault_nested(struct kvm_vcpu *vcpu,
		struct x86_exception *fault)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	WARN_ON(!is_guest_mode(vcpu));

	if (nested_vmx_is_page_fault_vmexit(vmcs12, fault->error_code)) {
		vmcs12->vm_exit_intr_error_code = fault->error_code;
		nested_vmx_vmexit(vcpu, EXIT_REASON_EXCEPTION_NMI,
				  PF_VECTOR | INTR_TYPE_HARD_EXCEPTION |
				  INTR_INFO_DELIVER_CODE_MASK | INTR_INFO_VALID_MASK,
				  fault->address);
	} else {
		kvm_inject_page_fault(vcpu, fault);
	}
}
