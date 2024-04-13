static int vmx_cpu_uses_apicv(struct kvm_vcpu *vcpu)
{
	return enable_apicv && lapic_in_kernel(vcpu);
}
