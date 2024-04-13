static inline dma_addr_t pci_map_single_debug(struct pci_dev *pdev, void *ptr,
					      size_t size, int direction)
{
	dma_addr_t addr = pci_map_single(pdev, ptr, size, direction);
	if (addr + size > 0x100000000LL)
		pr_crit("%s: pci_map_single() returned memory at 0x%llx!\n",
			pci_name(pdev), (unsigned long long)addr);
	return addr;
}
