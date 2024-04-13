static void tg3_rx_prodring_free(struct tg3 *tp,
				 struct tg3_rx_prodring_set *tpr)
{
	int i;

	if (tpr != &tp->napi[0].prodring) {
		for (i = tpr->rx_std_cons_idx; i != tpr->rx_std_prod_idx;
		     i = (i + 1) & tp->rx_std_ring_mask)
			tg3_rx_data_free(tp, &tpr->rx_std_buffers[i],
					tp->rx_pkt_map_sz);

		if (tg3_flag(tp, JUMBO_CAPABLE)) {
			for (i = tpr->rx_jmb_cons_idx;
			     i != tpr->rx_jmb_prod_idx;
			     i = (i + 1) & tp->rx_jmb_ring_mask) {
				tg3_rx_data_free(tp, &tpr->rx_jmb_buffers[i],
						TG3_RX_JMB_MAP_SZ);
			}
		}

		return;
	}

	for (i = 0; i <= tp->rx_std_ring_mask; i++)
		tg3_rx_data_free(tp, &tpr->rx_std_buffers[i],
				tp->rx_pkt_map_sz);

	if (tg3_flag(tp, JUMBO_CAPABLE) && !tg3_flag(tp, 5780_CLASS)) {
		for (i = 0; i <= tp->rx_jmb_ring_mask; i++)
			tg3_rx_data_free(tp, &tpr->rx_jmb_buffers[i],
					TG3_RX_JMB_MAP_SZ);
	}
}
