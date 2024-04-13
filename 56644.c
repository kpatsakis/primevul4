static int vmx_deliver_nested_posted_interrupt(struct kvm_vcpu *vcpu,
						int vector)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (is_guest_mode(vcpu) &&
	    vector == vmx->nested.posted_intr_nv) {
		/* the PIR and ON have been set by L1. */
		kvm_vcpu_trigger_posted_interrupt(vcpu, true);
		/*
		 * If a posted intr is not recognized by hardware,
		 * we will accomplish it in the next vmentry.
		 */
		vmx->nested.pi_pending = true;
		kvm_make_request(KVM_REQ_EVENT, vcpu);
		return 0;
	}
	return -1;
}
