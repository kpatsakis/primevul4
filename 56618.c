static int nested_vmx_eptp_switching(struct kvm_vcpu *vcpu,
				     struct vmcs12 *vmcs12)
{
	u32 index = vcpu->arch.regs[VCPU_REGS_RCX];
	u64 address;
	bool accessed_dirty;
	struct kvm_mmu *mmu = vcpu->arch.walk_mmu;

	if (!nested_cpu_has_eptp_switching(vmcs12) ||
	    !nested_cpu_has_ept(vmcs12))
		return 1;

	if (index >= VMFUNC_EPTP_ENTRIES)
		return 1;


	if (kvm_vcpu_read_guest_page(vcpu, vmcs12->eptp_list_address >> PAGE_SHIFT,
				     &address, index * 8, 8))
		return 1;

	accessed_dirty = !!(address & VMX_EPTP_AD_ENABLE_BIT);

	/*
	 * If the (L2) guest does a vmfunc to the currently
	 * active ept pointer, we don't have to do anything else
	 */
	if (vmcs12->ept_pointer != address) {
		if (!valid_ept_address(vcpu, address))
			return 1;

		kvm_mmu_unload(vcpu);
		mmu->ept_ad = accessed_dirty;
		mmu->base_role.ad_disabled = !accessed_dirty;
		vmcs12->ept_pointer = address;
		/*
		 * TODO: Check what's the correct approach in case
		 * mmu reload fails. Currently, we just let the next
		 * reload potentially fail
		 */
		kvm_mmu_reload(vcpu);
	}

	return 0;
}
