static int tg3_mem_tx_acquire(struct tg3 *tp)
{
	int i;
	struct tg3_napi *tnapi = &tp->napi[0];

	/* If multivector TSS is enabled, vector 0 does not handle
	 * tx interrupts.  Don't allocate any resources for it.
	 */
	if (tg3_flag(tp, ENABLE_TSS))
		tnapi++;

	for (i = 0; i < tp->txq_cnt; i++, tnapi++) {
		tnapi->tx_buffers = kzalloc(sizeof(struct tg3_tx_ring_info) *
					    TG3_TX_RING_SIZE, GFP_KERNEL);
		if (!tnapi->tx_buffers)
			goto err_out;

		tnapi->tx_ring = dma_alloc_coherent(&tp->pdev->dev,
						    TG3_TX_RING_BYTES,
						    &tnapi->tx_desc_mapping,
						    GFP_KERNEL);
		if (!tnapi->tx_ring)
			goto err_out;
	}

	return 0;

err_out:
	tg3_mem_tx_release(tp);
	return -ENOMEM;
}
