static int tigon3_dma_hwbug_workaround(struct tg3_napi *tnapi,
				       struct sk_buff **pskb,
				       u32 *entry, u32 *budget,
				       u32 base_flags, u32 mss, u32 vlan)
{
	struct tg3 *tp = tnapi->tp;
	struct sk_buff *new_skb, *skb = *pskb;
	dma_addr_t new_addr = 0;
	int ret = 0;

	if (tg3_asic_rev(tp) != ASIC_REV_5701)
		new_skb = skb_copy(skb, GFP_ATOMIC);
	else {
		int more_headroom = 4 - ((unsigned long)skb->data & 3);

		new_skb = skb_copy_expand(skb,
					  skb_headroom(skb) + more_headroom,
					  skb_tailroom(skb), GFP_ATOMIC);
	}

	if (!new_skb) {
		ret = -1;
	} else {
		/* New SKB is guaranteed to be linear. */
		new_addr = pci_map_single(tp->pdev, new_skb->data, new_skb->len,
					  PCI_DMA_TODEVICE);
		/* Make sure the mapping succeeded */
		if (pci_dma_mapping_error(tp->pdev, new_addr)) {
			dev_kfree_skb(new_skb);
			ret = -1;
		} else {
			u32 save_entry = *entry;

			base_flags |= TXD_FLAG_END;

			tnapi->tx_buffers[*entry].skb = new_skb;
			dma_unmap_addr_set(&tnapi->tx_buffers[*entry],
					   mapping, new_addr);

			if (tg3_tx_frag_set(tnapi, entry, budget, new_addr,
					    new_skb->len, base_flags,
					    mss, vlan)) {
				tg3_tx_skb_unmap(tnapi, save_entry, -1);
				dev_kfree_skb(new_skb);
				ret = -1;
			}
		}
	}

	dev_kfree_skb(skb);
	*pskb = new_skb;
	return ret;
}
