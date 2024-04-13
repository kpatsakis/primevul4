static int handle_vmptrld(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	gpa_t vmptr;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (nested_vmx_get_vmptr(vcpu, &vmptr))
		return 1;

	if (!PAGE_ALIGNED(vmptr) || (vmptr >> cpuid_maxphyaddr(vcpu))) {
		nested_vmx_failValid(vcpu, VMXERR_VMPTRLD_INVALID_ADDRESS);
		return kvm_skip_emulated_instruction(vcpu);
	}

	if (vmptr == vmx->nested.vmxon_ptr) {
		nested_vmx_failValid(vcpu, VMXERR_VMPTRLD_VMXON_POINTER);
		return kvm_skip_emulated_instruction(vcpu);
	}

	if (vmx->nested.current_vmptr != vmptr) {
		struct vmcs12 *new_vmcs12;
		struct page *page;
		page = kvm_vcpu_gpa_to_page(vcpu, vmptr);
		if (is_error_page(page)) {
			nested_vmx_failInvalid(vcpu);
			return kvm_skip_emulated_instruction(vcpu);
		}
		new_vmcs12 = kmap(page);
		if (new_vmcs12->revision_id != VMCS12_REVISION) {
			kunmap(page);
			kvm_release_page_clean(page);
			nested_vmx_failValid(vcpu,
				VMXERR_VMPTRLD_INCORRECT_VMCS_REVISION_ID);
			return kvm_skip_emulated_instruction(vcpu);
		}

		nested_release_vmcs12(vmx);
		/*
		 * Load VMCS12 from guest memory since it is not already
		 * cached.
		 */
		memcpy(vmx->nested.cached_vmcs12, new_vmcs12, VMCS12_SIZE);
		kunmap(page);
		kvm_release_page_clean(page);

		set_current_vmptr(vmx, vmptr);
	}

	nested_vmx_succeed(vcpu);
	return kvm_skip_emulated_instruction(vcpu);
}
