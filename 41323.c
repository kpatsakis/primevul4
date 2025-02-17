static void tg3_free_consistent(struct tg3 *tp)
{
	int i;

	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		if (tnapi->hw_status) {
			dma_free_coherent(&tp->pdev->dev, TG3_HW_STATUS_SIZE,
					  tnapi->hw_status,
					  tnapi->status_mapping);
			tnapi->hw_status = NULL;
		}
	}

	tg3_mem_rx_release(tp);
	tg3_mem_tx_release(tp);

	if (tp->hw_stats) {
		dma_free_coherent(&tp->pdev->dev, sizeof(struct tg3_hw_stats),
				  tp->hw_stats, tp->stats_mapping);
		tp->hw_stats = NULL;
	}
}
