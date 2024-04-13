static void tg3_mem_rx_release(struct tg3 *tp)
{
	int i;

	for (i = 0; i < tp->irq_max; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		tg3_rx_prodring_fini(tp, &tnapi->prodring);

		if (!tnapi->rx_rcb)
			continue;

		dma_free_coherent(&tp->pdev->dev,
				  TG3_RX_RCB_RING_BYTES(tp),
				  tnapi->rx_rcb,
				  tnapi->rx_rcb_mapping);
		tnapi->rx_rcb = NULL;
	}
}
