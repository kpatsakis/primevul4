static int tg3_rx_prodring_alloc(struct tg3 *tp,
				 struct tg3_rx_prodring_set *tpr)
{
	u32 i, rx_pkt_dma_sz;

	tpr->rx_std_cons_idx = 0;
	tpr->rx_std_prod_idx = 0;
	tpr->rx_jmb_cons_idx = 0;
	tpr->rx_jmb_prod_idx = 0;

	if (tpr != &tp->napi[0].prodring) {
		memset(&tpr->rx_std_buffers[0], 0,
		       TG3_RX_STD_BUFF_RING_SIZE(tp));
		if (tpr->rx_jmb_buffers)
			memset(&tpr->rx_jmb_buffers[0], 0,
			       TG3_RX_JMB_BUFF_RING_SIZE(tp));
		goto done;
	}

	/* Zero out all descriptors. */
	memset(tpr->rx_std, 0, TG3_RX_STD_RING_BYTES(tp));

	rx_pkt_dma_sz = TG3_RX_STD_DMA_SZ;
	if (tg3_flag(tp, 5780_CLASS) &&
	    tp->dev->mtu > ETH_DATA_LEN)
		rx_pkt_dma_sz = TG3_RX_JMB_DMA_SZ;
	tp->rx_pkt_map_sz = TG3_RX_DMA_TO_MAP_SZ(rx_pkt_dma_sz);

	/* Initialize invariants of the rings, we only set this
	 * stuff once.  This works because the card does not
	 * write into the rx buffer posting rings.
	 */
	for (i = 0; i <= tp->rx_std_ring_mask; i++) {
		struct tg3_rx_buffer_desc *rxd;

		rxd = &tpr->rx_std[i];
		rxd->idx_len = rx_pkt_dma_sz << RXD_LEN_SHIFT;
		rxd->type_flags = (RXD_FLAG_END << RXD_FLAGS_SHIFT);
		rxd->opaque = (RXD_OPAQUE_RING_STD |
			       (i << RXD_OPAQUE_INDEX_SHIFT));
	}

	/* Now allocate fresh SKBs for each rx ring. */
	for (i = 0; i < tp->rx_pending; i++) {
		unsigned int frag_size;

		if (tg3_alloc_rx_data(tp, tpr, RXD_OPAQUE_RING_STD, i,
				      &frag_size) < 0) {
			netdev_warn(tp->dev,
				    "Using a smaller RX standard ring. Only "
				    "%d out of %d buffers were allocated "
				    "successfully\n", i, tp->rx_pending);
			if (i == 0)
				goto initfail;
			tp->rx_pending = i;
			break;
		}
	}

	if (!tg3_flag(tp, JUMBO_CAPABLE) || tg3_flag(tp, 5780_CLASS))
		goto done;

	memset(tpr->rx_jmb, 0, TG3_RX_JMB_RING_BYTES(tp));

	if (!tg3_flag(tp, JUMBO_RING_ENABLE))
		goto done;

	for (i = 0; i <= tp->rx_jmb_ring_mask; i++) {
		struct tg3_rx_buffer_desc *rxd;

		rxd = &tpr->rx_jmb[i].std;
		rxd->idx_len = TG3_RX_JMB_DMA_SZ << RXD_LEN_SHIFT;
		rxd->type_flags = (RXD_FLAG_END << RXD_FLAGS_SHIFT) |
				  RXD_FLAG_JUMBO;
		rxd->opaque = (RXD_OPAQUE_RING_JUMBO |
		       (i << RXD_OPAQUE_INDEX_SHIFT));
	}

	for (i = 0; i < tp->rx_jumbo_pending; i++) {
		unsigned int frag_size;

		if (tg3_alloc_rx_data(tp, tpr, RXD_OPAQUE_RING_JUMBO, i,
				      &frag_size) < 0) {
			netdev_warn(tp->dev,
				    "Using a smaller RX jumbo ring. Only %d "
				    "out of %d buffers were allocated "
				    "successfully\n", i, tp->rx_jumbo_pending);
			if (i == 0)
				goto initfail;
			tp->rx_jumbo_pending = i;
			break;
		}
	}

done:
	return 0;

initfail:
	tg3_rx_prodring_free(tp, tpr);
	return -ENOMEM;
}
