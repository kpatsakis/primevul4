void kvm_set_pending_timer(struct kvm_vcpu *vcpu)
{
	/*
	 * Note: KVM_REQ_PENDING_TIMER is implicitly checked in
	 * vcpu_enter_guest.  This function is only called from
	 * the physical CPU that is running vcpu.
	 */
	kvm_make_request(KVM_REQ_PENDING_TIMER, vcpu);
}
