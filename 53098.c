static s32 atl2_setup_ring_resources(struct atl2_adapter *adapter)
{
	struct pci_dev *pdev = adapter->pdev;
	int size;
	u8 offset = 0;

	/* real ring DMA buffer */
	adapter->ring_size = size =
		adapter->txd_ring_size * 1 + 7 +	/* dword align */
		adapter->txs_ring_size * 4 + 7 +	/* dword align */
		adapter->rxd_ring_size * 1536 + 127;	/* 128bytes align */

	adapter->ring_vir_addr = pci_alloc_consistent(pdev, size,
		&adapter->ring_dma);
	if (!adapter->ring_vir_addr)
		return -ENOMEM;
	memset(adapter->ring_vir_addr, 0, adapter->ring_size);

	/* Init TXD Ring */
	adapter->txd_dma = adapter->ring_dma ;
	offset = (adapter->txd_dma & 0x7) ? (8 - (adapter->txd_dma & 0x7)) : 0;
	adapter->txd_dma += offset;
	adapter->txd_ring = adapter->ring_vir_addr + offset;

	/* Init TXS Ring */
	adapter->txs_dma = adapter->txd_dma + adapter->txd_ring_size;
	offset = (adapter->txs_dma & 0x7) ? (8 - (adapter->txs_dma & 0x7)) : 0;
	adapter->txs_dma += offset;
	adapter->txs_ring = (struct tx_pkt_status *)
		(((u8 *)adapter->txd_ring) + (adapter->txd_ring_size + offset));

	/* Init RXD Ring */
	adapter->rxd_dma = adapter->txs_dma + adapter->txs_ring_size * 4;
	offset = (adapter->rxd_dma & 127) ?
		(128 - (adapter->rxd_dma & 127)) : 0;
	if (offset > 7)
		offset -= 8;
	else
		offset += (128 - 8);

	adapter->rxd_dma += offset;
	adapter->rxd_ring = (struct rx_desc *) (((u8 *)adapter->txs_ring) +
		(adapter->txs_ring_size * 4 + offset));

/*
 * Read / Write Ptr Initialize:
 *      init_ring_ptrs(adapter);
 */
	return 0;
}
