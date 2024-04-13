static int vmx_nmi_allowed(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu)) {
		struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

		if (to_vmx(vcpu)->nested.nested_run_pending)
			return 0;
		if (nested_exit_on_nmi(vcpu)) {
			nested_vmx_vmexit(vcpu);
			vmcs12->vm_exit_reason = EXIT_REASON_EXCEPTION_NMI;
			vmcs12->vm_exit_intr_info = NMI_VECTOR |
				INTR_TYPE_NMI_INTR | INTR_INFO_VALID_MASK;
			/*
			 * The NMI-triggered VM exit counts as injection:
			 * clear this one and block further NMIs.
			 */
			vcpu->arch.nmi_pending = 0;
			vmx_set_nmi_mask(vcpu, true);
			return 0;
		}
	}

	if (!cpu_has_virtual_nmis() && to_vmx(vcpu)->soft_vnmi_blocked)
		return 0;

	return	!(vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
		  (GUEST_INTR_STATE_MOV_SS | GUEST_INTR_STATE_STI
		   | GUEST_INTR_STATE_NMI));
}
