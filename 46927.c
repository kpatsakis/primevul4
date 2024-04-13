static void vmx_hwapic_irr_update(struct kvm_vcpu *vcpu, int max_irr)
{
	if (!is_guest_mode(vcpu)) {
		vmx_set_rvi(max_irr);
		return;
	}

	if (max_irr == -1)
		return;

	/*
	 * In guest mode.  If a vmexit is needed, vmx_check_nested_events
	 * handles it.
	 */
	if (nested_exit_on_intr(vcpu))
		return;

	/*
	 * Else, fall back to pre-APICv interrupt injection since L2
	 * is run without virtual interrupt delivery.
	 */
	if (!kvm_event_needs_reinjection(vcpu) &&
	    vmx_interrupt_allowed(vcpu)) {
		kvm_queue_interrupt(vcpu, max_irr, false);
		vmx_inject_irq(vcpu);
	}
}
