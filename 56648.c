void vmx_enable_tdp(void)
{
	kvm_mmu_set_mask_ptes(VMX_EPT_READABLE_MASK,
		enable_ept_ad_bits ? VMX_EPT_ACCESS_BIT : 0ull,
		enable_ept_ad_bits ? VMX_EPT_DIRTY_BIT : 0ull,
		0ull, VMX_EPT_EXECUTABLE_MASK,
		cpu_has_vmx_ept_execute_only() ? 0ull : VMX_EPT_READABLE_MASK,
		VMX_EPT_RWX_MASK, 0ull);

	ept_set_mmio_spte_mask();
	kvm_enable_tdp();
}
