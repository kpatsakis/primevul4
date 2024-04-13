static int vmx_interrupt_allowed(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu)) {
		struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

		if (to_vmx(vcpu)->nested.nested_run_pending)
			return 0;
		if (nested_exit_on_intr(vcpu)) {
			nested_vmx_vmexit(vcpu);
			vmcs12->vm_exit_reason =
				EXIT_REASON_EXTERNAL_INTERRUPT;
			vmcs12->vm_exit_intr_info = 0;
			/*
			 * fall through to normal code, but now in L1, not L2
			 */
		}
	}

	return (vmcs_readl(GUEST_RFLAGS) & X86_EFLAGS_IF) &&
		!(vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
			(GUEST_INTR_STATE_STI | GUEST_INTR_STATE_MOV_SS));
}
