static int nested_vmx_msr_check_common(struct kvm_vcpu *vcpu,
				       struct vmx_msr_entry *e)
{
	/* x2APIC MSR accesses are not allowed */
	if (vcpu->arch.apic_base & X2APIC_ENABLE && e->index >> 8 == 0x8)
		return -EINVAL;
	if (e->index == MSR_IA32_UCODE_WRITE || /* SDM Table 35-2 */
	    e->index == MSR_IA32_UCODE_REV)
		return -EINVAL;
	if (e->reserved != 0)
		return -EINVAL;
	return 0;
}
