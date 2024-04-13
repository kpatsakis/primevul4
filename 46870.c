static bool nested_cr0_valid(struct kvm_vcpu *vcpu, unsigned long val)
{
	unsigned long always_on = VMXON_CR0_ALWAYSON;
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	if (to_vmx(vcpu)->nested.nested_vmx_secondary_ctls_high &
		SECONDARY_EXEC_UNRESTRICTED_GUEST &&
	    nested_cpu_has2(vmcs12, SECONDARY_EXEC_UNRESTRICTED_GUEST))
		always_on &= ~(X86_CR0_PE | X86_CR0_PG);
	return (val & always_on) == always_on;
}
