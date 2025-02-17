static int tg3_mem_rx_acquire(struct tg3 *tp)
{
	unsigned int i, limit;

	limit = tp->rxq_cnt;

	/* If RSS is enabled, we need a (dummy) producer ring
	 * set on vector zero.  This is the true hw prodring.
	 */
	if (tg3_flag(tp, ENABLE_RSS))
		limit++;

	for (i = 0; i < limit; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		if (tg3_rx_prodring_init(tp, &tnapi->prodring))
			goto err_out;

		/* If multivector RSS is enabled, vector 0
		 * does not handle rx or tx interrupts.
		 * Don't allocate any resources for it.
		 */
		if (!i && tg3_flag(tp, ENABLE_RSS))
			continue;

		tnapi->rx_rcb = dma_alloc_coherent(&tp->pdev->dev,
						   TG3_RX_RCB_RING_BYTES(tp),
						   &tnapi->rx_rcb_mapping,
						   GFP_KERNEL);
		if (!tnapi->rx_rcb)
			goto err_out;

		memset(tnapi->rx_rcb, 0, TG3_RX_RCB_RING_BYTES(tp));
	}

	return 0;

err_out:
	tg3_mem_rx_release(tp);
	return -ENOMEM;
}
