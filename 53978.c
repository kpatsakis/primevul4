bool kvm_vcpu_is_bsp(struct kvm_vcpu *vcpu)
{
	return (vcpu->arch.apic_base & MSR_IA32_APICBASE_BSP) != 0;
}
