bool kvm_vcpu_compatible(struct kvm_vcpu *vcpu)
{
	return irqchip_in_kernel(vcpu->kvm) == lapic_in_kernel(vcpu);
}
