static int tg3_alloc_consistent(struct tg3 *tp)
{
	int i;

	tp->hw_stats = dma_alloc_coherent(&tp->pdev->dev,
					  sizeof(struct tg3_hw_stats),
					  &tp->stats_mapping,
					  GFP_KERNEL);
	if (!tp->hw_stats)
		goto err_out;

	memset(tp->hw_stats, 0, sizeof(struct tg3_hw_stats));

	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];
		struct tg3_hw_status *sblk;

		tnapi->hw_status = dma_alloc_coherent(&tp->pdev->dev,
						      TG3_HW_STATUS_SIZE,
						      &tnapi->status_mapping,
						      GFP_KERNEL);
		if (!tnapi->hw_status)
			goto err_out;

		memset(tnapi->hw_status, 0, TG3_HW_STATUS_SIZE);
		sblk = tnapi->hw_status;

		if (tg3_flag(tp, ENABLE_RSS)) {
			u16 *prodptr = NULL;

			/*
			 * When RSS is enabled, the status block format changes
			 * slightly.  The "rx_jumbo_consumer", "reserved",
			 * and "rx_mini_consumer" members get mapped to the
			 * other three rx return ring producer indexes.
			 */
			switch (i) {
			case 1:
				prodptr = &sblk->idx[0].rx_producer;
				break;
			case 2:
				prodptr = &sblk->rx_jumbo_consumer;
				break;
			case 3:
				prodptr = &sblk->reserved;
				break;
			case 4:
				prodptr = &sblk->rx_mini_consumer;
				break;
			}
			tnapi->rx_rcb_prod_idx = prodptr;
		} else {
			tnapi->rx_rcb_prod_idx = &sblk->idx[0].rx_producer;
		}
	}

	if (tg3_mem_tx_acquire(tp) || tg3_mem_rx_acquire(tp))
		goto err_out;

	return 0;

err_out:
	tg3_free_consistent(tp);
	return -ENOMEM;
}
