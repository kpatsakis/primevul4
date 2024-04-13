static int get_ept_level(struct kvm_vcpu *vcpu)
{
	if (cpu_has_vmx_ept_5levels() && (cpuid_maxphyaddr(vcpu) > 48))
		return 5;
	return 4;
}
