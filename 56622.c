static void nested_vmx_inject_exception_vmexit(struct kvm_vcpu *vcpu,
					       unsigned long exit_qual)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	unsigned int nr = vcpu->arch.exception.nr;
	u32 intr_info = nr | INTR_INFO_VALID_MASK;

	if (vcpu->arch.exception.has_error_code) {
		vmcs12->vm_exit_intr_error_code = vcpu->arch.exception.error_code;
		intr_info |= INTR_INFO_DELIVER_CODE_MASK;
	}

	if (kvm_exception_is_soft(nr))
		intr_info |= INTR_TYPE_SOFT_EXCEPTION;
	else
		intr_info |= INTR_TYPE_HARD_EXCEPTION;

	if (!(vmcs12->idt_vectoring_info_field & VECTORING_INFO_VALID_MASK) &&
	    vmx_get_nmi_mask(vcpu))
		intr_info |= INTR_INFO_UNBLOCK_NMI;

	nested_vmx_vmexit(vcpu, EXIT_REASON_EXCEPTION_NMI, intr_info, exit_qual);
}
