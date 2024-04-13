static int nested_vmx_reflect_vmexit(struct kvm_vcpu *vcpu, u32 exit_reason)
{
	u32 exit_intr_info = vmcs_read32(VM_EXIT_INTR_INFO);

	/*
	 * At this point, the exit interruption info in exit_intr_info
	 * is only valid for EXCEPTION_NMI exits.  For EXTERNAL_INTERRUPT
	 * we need to query the in-kernel LAPIC.
	 */
	WARN_ON(exit_reason == EXIT_REASON_EXTERNAL_INTERRUPT);
	if ((exit_intr_info &
	     (INTR_INFO_VALID_MASK | INTR_INFO_DELIVER_CODE_MASK)) ==
	    (INTR_INFO_VALID_MASK | INTR_INFO_DELIVER_CODE_MASK)) {
		struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
		vmcs12->vm_exit_intr_error_code =
			vmcs_read32(VM_EXIT_INTR_ERROR_CODE);
	}

	nested_vmx_vmexit(vcpu, exit_reason, exit_intr_info,
			  vmcs_readl(EXIT_QUALIFICATION));
	return 1;
}
