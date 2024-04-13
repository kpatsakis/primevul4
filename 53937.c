unsigned long kvm_get_cr8(struct kvm_vcpu *vcpu)
{
	if (lapic_in_kernel(vcpu))
		return kvm_lapic_get_cr8(vcpu);
	else
		return vcpu->arch.cr8;
}
