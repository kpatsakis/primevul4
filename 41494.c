static void tg3_rx_data_free(struct tg3 *tp, struct ring_info *ri, u32 map_sz)
{
	unsigned int skb_size = SKB_DATA_ALIGN(map_sz + TG3_RX_OFFSET(tp)) +
		   SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	if (!ri->data)
		return;

	pci_unmap_single(tp->pdev, dma_unmap_addr(ri, mapping),
			 map_sz, PCI_DMA_FROMDEVICE);
	tg3_frag_free(skb_size <= PAGE_SIZE, ri->data);
	ri->data = NULL;
}
