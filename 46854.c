static int handle_wbinvd(struct kvm_vcpu *vcpu)
{
	kvm_emulate_wbinvd(vcpu);
	return 1;
}
