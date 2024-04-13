static int nested_vmx_check_pml_controls(struct kvm_vcpu *vcpu,
					 struct vmcs12 *vmcs12)
{
	u64 address = vmcs12->pml_address;
	int maxphyaddr = cpuid_maxphyaddr(vcpu);

	if (nested_cpu_has2(vmcs12, SECONDARY_EXEC_ENABLE_PML)) {
		if (!nested_cpu_has_ept(vmcs12) ||
		    !IS_ALIGNED(address, 4096)  ||
		    address >> maxphyaddr)
			return -EINVAL;
	}

	return 0;
}
