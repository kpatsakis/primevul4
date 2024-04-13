static void vmx_inject_page_fault_nested(struct kvm_vcpu *vcpu,
		struct x86_exception *fault)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	WARN_ON(!is_guest_mode(vcpu));

	if (nested_vmx_is_page_fault_vmexit(vmcs12, fault->error_code))
		nested_vmx_vmexit(vcpu, to_vmx(vcpu)->exit_reason,
				  vmcs_read32(VM_EXIT_INTR_INFO),
				  vmcs_readl(EXIT_QUALIFICATION));
	else
		kvm_inject_page_fault(vcpu, fault);
}
