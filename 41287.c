static int tg3_alloc_rx_data(struct tg3 *tp, struct tg3_rx_prodring_set *tpr,
			     u32 opaque_key, u32 dest_idx_unmasked,
			     unsigned int *frag_size)
{
	struct tg3_rx_buffer_desc *desc;
	struct ring_info *map;
	u8 *data;
	dma_addr_t mapping;
	int skb_size, data_size, dest_idx;

	switch (opaque_key) {
	case RXD_OPAQUE_RING_STD:
		dest_idx = dest_idx_unmasked & tp->rx_std_ring_mask;
		desc = &tpr->rx_std[dest_idx];
		map = &tpr->rx_std_buffers[dest_idx];
		data_size = tp->rx_pkt_map_sz;
		break;

	case RXD_OPAQUE_RING_JUMBO:
		dest_idx = dest_idx_unmasked & tp->rx_jmb_ring_mask;
		desc = &tpr->rx_jmb[dest_idx].std;
		map = &tpr->rx_jmb_buffers[dest_idx];
		data_size = TG3_RX_JMB_MAP_SZ;
		break;

	default:
		return -EINVAL;
	}

	/* Do not overwrite any of the map or rp information
	 * until we are sure we can commit to a new buffer.
	 *
	 * Callers depend upon this behavior and assume that
	 * we leave everything unchanged if we fail.
	 */
	skb_size = SKB_DATA_ALIGN(data_size + TG3_RX_OFFSET(tp)) +
		   SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	if (skb_size <= PAGE_SIZE) {
		data = netdev_alloc_frag(skb_size);
		*frag_size = skb_size;
	} else {
		data = kmalloc(skb_size, GFP_ATOMIC);
		*frag_size = 0;
	}
	if (!data)
		return -ENOMEM;

	mapping = pci_map_single(tp->pdev,
				 data + TG3_RX_OFFSET(tp),
				 data_size,
				 PCI_DMA_FROMDEVICE);
	if (unlikely(pci_dma_mapping_error(tp->pdev, mapping))) {
		tg3_frag_free(skb_size <= PAGE_SIZE, data);
		return -EIO;
	}

	map->data = data;
	dma_unmap_addr_set(map, mapping, mapping);

	desc->addr_hi = ((u64)mapping >> 32);
	desc->addr_lo = ((u64)mapping & 0xffffffff);

	return data_size;
}
