static inline void nested_release_vmcs12(struct vcpu_vmx *vmx)
{
	if (vmx->nested.current_vmptr == -1ull)
		return;

	/* current_vmptr and current_vmcs12 are always set/reset together */
	if (WARN_ON(vmx->nested.current_vmcs12 == NULL))
		return;

	if (enable_shadow_vmcs) {
		/* copy to memory all shadowed fields in case
		   they were modified */
		copy_shadow_to_vmcs12(vmx);
		vmx->nested.sync_shadow_vmcs = false;
		vmcs_clear_bits(SECONDARY_VM_EXEC_CONTROL,
				SECONDARY_EXEC_SHADOW_VMCS);
		vmcs_write64(VMCS_LINK_POINTER, -1ull);
	}
	vmx->nested.posted_intr_nv = -1;
	kunmap(vmx->nested.current_vmcs12_page);
	nested_release_page(vmx->nested.current_vmcs12_page);
	vmx->nested.current_vmptr = -1ull;
	vmx->nested.current_vmcs12 = NULL;
}
