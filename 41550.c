static void tg3_tx_skb_unmap(struct tg3_napi *tnapi, u32 entry, int last)
{
	int i;
	struct sk_buff *skb;
	struct tg3_tx_ring_info *txb = &tnapi->tx_buffers[entry];

	skb = txb->skb;
	txb->skb = NULL;

	pci_unmap_single(tnapi->tp->pdev,
			 dma_unmap_addr(txb, mapping),
			 skb_headlen(skb),
			 PCI_DMA_TODEVICE);

	while (txb->fragmented) {
		txb->fragmented = false;
		entry = NEXT_TX(entry);
		txb = &tnapi->tx_buffers[entry];
	}

	for (i = 0; i <= last; i++) {
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		entry = NEXT_TX(entry);
		txb = &tnapi->tx_buffers[entry];

		pci_unmap_page(tnapi->tp->pdev,
			       dma_unmap_addr(txb, mapping),
			       skb_frag_size(frag), PCI_DMA_TODEVICE);

		while (txb->fragmented) {
			txb->fragmented = false;
			entry = NEXT_TX(entry);
			txb = &tnapi->tx_buffers[entry];
		}
	}
}
