static void vmx_deliver_posted_interrupt(struct kvm_vcpu *vcpu, int vector)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int r;

	r = vmx_deliver_nested_posted_interrupt(vcpu, vector);
	if (!r)
		return;

	if (pi_test_and_set_pir(vector, &vmx->pi_desc))
		return;

	r = pi_test_and_set_on(&vmx->pi_desc);
	kvm_make_request(KVM_REQ_EVENT, vcpu);
	if (r || !kvm_vcpu_trigger_posted_interrupt(vcpu))
		kvm_vcpu_kick(vcpu);
}
