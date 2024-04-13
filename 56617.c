static int nested_vmx_check_tpr_shadow_controls(struct kvm_vcpu *vcpu,
						struct vmcs12 *vmcs12)
{
	if (!nested_cpu_has(vmcs12, CPU_BASED_TPR_SHADOW))
		return 0;

	if (!page_address_valid(vcpu, vmcs12->virtual_apic_page_addr))
		return -EINVAL;

	return 0;
}
