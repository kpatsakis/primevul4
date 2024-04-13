static int vmx_write_pml_buffer(struct kvm_vcpu *vcpu)
{
	struct vmcs12 *vmcs12;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	gpa_t gpa;
	struct page *page = NULL;
	u64 *pml_address;

	if (is_guest_mode(vcpu)) {
		WARN_ON_ONCE(vmx->nested.pml_full);

		/*
		 * Check if PML is enabled for the nested guest.
		 * Whether eptp bit 6 is set is already checked
		 * as part of A/D emulation.
		 */
		vmcs12 = get_vmcs12(vcpu);
		if (!nested_cpu_has_pml(vmcs12))
			return 0;

		if (vmcs12->guest_pml_index >= PML_ENTITY_NUM) {
			vmx->nested.pml_full = true;
			return 1;
		}

		gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS) & ~0xFFFull;

		page = kvm_vcpu_gpa_to_page(vcpu, vmcs12->pml_address);
		if (is_error_page(page))
			return 0;

		pml_address = kmap(page);
		pml_address[vmcs12->guest_pml_index--] = gpa;
		kunmap(page);
		kvm_release_page_clean(page);
	}

	return 0;
}
