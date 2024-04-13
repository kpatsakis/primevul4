static void free_nested(struct vcpu_vmx *vmx)
{
	if (!vmx->nested.vmxon)
		return;
	vmx->nested.vmxon = false;
	if (vmx->nested.current_vmptr != -1ull) {
		nested_release_vmcs12(vmx);
		vmx->nested.current_vmptr = -1ull;
		vmx->nested.current_vmcs12 = NULL;
	}
	if (enable_shadow_vmcs)
		free_vmcs(vmx->nested.current_shadow_vmcs);
	/* Unpin physical memory we referred to in current vmcs02 */
	if (vmx->nested.apic_access_page) {
		nested_release_page(vmx->nested.apic_access_page);
		vmx->nested.apic_access_page = 0;
	}

	nested_free_all_saved_vmcss(vmx);
}
