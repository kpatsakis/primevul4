static int nested_vmx_check_exception(struct kvm_vcpu *vcpu, unsigned long *exit_qual)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	unsigned int nr = vcpu->arch.exception.nr;

	if (nr == PF_VECTOR) {
		if (vcpu->arch.exception.nested_apf) {
			*exit_qual = vcpu->arch.apf.nested_apf_token;
			return 1;
		}
		/*
		 * FIXME: we must not write CR2 when L1 intercepts an L2 #PF exception.
		 * The fix is to add the ancillary datum (CR2 or DR6) to structs
		 * kvm_queued_exception and kvm_vcpu_events, so that CR2 and DR6
		 * can be written only when inject_pending_event runs.  This should be
		 * conditional on a new capability---if the capability is disabled,
		 * kvm_multiple_exception would write the ancillary information to
		 * CR2 or DR6, for backwards ABI-compatibility.
		 */
		if (nested_vmx_is_page_fault_vmexit(vmcs12,
						    vcpu->arch.exception.error_code)) {
			*exit_qual = vcpu->arch.cr2;
			return 1;
		}
	} else {
		if (vmcs12->exception_bitmap & (1u << nr)) {
			if (nr == DB_VECTOR)
				*exit_qual = vcpu->arch.dr6;
			else
				*exit_qual = 0;
			return 1;
		}
	}

	return 0;
}
