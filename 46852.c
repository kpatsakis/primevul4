static int handle_vmcall(struct kvm_vcpu *vcpu)
{
	kvm_emulate_hypercall(vcpu);
	return 1;
}
