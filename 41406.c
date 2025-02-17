static void tg3_mem_tx_release(struct tg3 *tp)
{
	int i;

	for (i = 0; i < tp->irq_max; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		if (tnapi->tx_ring) {
			dma_free_coherent(&tp->pdev->dev, TG3_TX_RING_BYTES,
				tnapi->tx_ring, tnapi->tx_desc_mapping);
			tnapi->tx_ring = NULL;
		}

		kfree(tnapi->tx_buffers);
		tnapi->tx_buffers = NULL;
	}
}
