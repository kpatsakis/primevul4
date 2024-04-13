void kvm_mmu_set_mmio_spte_mask(u64 mmio_mask)
{
	shadow_mmio_mask = mmio_mask;
}
