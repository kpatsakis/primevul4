static void kvm_set_mmio_spte_mask(void)
{
	u64 mask;
	int maxphyaddr = boot_cpu_data.x86_phys_bits;

	/*
	 * Set the reserved bits and the present bit of an paging-structure
	 * entry to generate page fault with PFER.RSV = 1.
	 */
	 /* Mask the reserved physical address bits. */
	mask = rsvd_bits(maxphyaddr, 51);

	/* Bit 62 is always reserved for 32bit host. */
	mask |= 0x3ull << 62;

	/* Set the present bit. */
	mask |= 1ull;

#ifdef CONFIG_X86_64
	/*
	 * If reserved bit is not supported, clear the present bit to disable
	 * mmio page fault.
	 */
	if (maxphyaddr == 52)
		mask &= ~1ull;
#endif

	kvm_mmu_set_mmio_spte_mask(mask);
}
