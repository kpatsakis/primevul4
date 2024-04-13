static int tg3_init_rings(struct tg3 *tp)
{
	int i;

	/* Free up all the SKBs. */
	tg3_free_rings(tp);

	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		tnapi->last_tag = 0;
		tnapi->last_irq_tag = 0;
		tnapi->hw_status->status = 0;
		tnapi->hw_status->status_tag = 0;
		memset(tnapi->hw_status, 0, TG3_HW_STATUS_SIZE);

		tnapi->tx_prod = 0;
		tnapi->tx_cons = 0;
		if (tnapi->tx_ring)
			memset(tnapi->tx_ring, 0, TG3_TX_RING_BYTES);

		tnapi->rx_rcb_ptr = 0;
		if (tnapi->rx_rcb)
			memset(tnapi->rx_rcb, 0, TG3_RX_RCB_RING_BYTES(tp));

		if (tg3_rx_prodring_alloc(tp, &tnapi->prodring)) {
			tg3_free_rings(tp);
			return -ENOMEM;
		}
	}

	return 0;
}
