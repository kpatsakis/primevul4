static inline void nested_release_vmcs12(struct vcpu_vmx *vmx)
{
	if (vmx->nested.current_vmptr == -1ull)
		return;

	if (enable_shadow_vmcs) {
		/* copy to memory all shadowed fields in case
		   they were modified */
		copy_shadow_to_vmcs12(vmx);
		vmx->nested.sync_shadow_vmcs = false;
		vmx_disable_shadow_vmcs(vmx);
	}
	vmx->nested.posted_intr_nv = -1;

	/* Flush VMCS12 to guest memory */
	kvm_vcpu_write_guest_page(&vmx->vcpu,
				  vmx->nested.current_vmptr >> PAGE_SHIFT,
				  vmx->nested.cached_vmcs12, 0, VMCS12_SIZE);

	vmx->nested.current_vmptr = -1ull;
}
