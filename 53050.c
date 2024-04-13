static void atl2_free_ring_resources(struct atl2_adapter *adapter)
{
	struct pci_dev *pdev = adapter->pdev;
	pci_free_consistent(pdev, adapter->ring_size, adapter->ring_vir_addr,
		adapter->ring_dma);
}
