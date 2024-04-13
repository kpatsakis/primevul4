static void tg3_chk_missed_msi(struct tg3 *tp)
{
	u32 i;

	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		if (tg3_has_work(tnapi)) {
			if (tnapi->last_rx_cons == tnapi->rx_rcb_ptr &&
			    tnapi->last_tx_cons == tnapi->tx_cons) {
				if (tnapi->chk_msi_cnt < 1) {
					tnapi->chk_msi_cnt++;
					return;
				}
				tg3_msi(0, tnapi);
			}
		}
		tnapi->chk_msi_cnt = 0;
		tnapi->last_rx_cons = tnapi->rx_rcb_ptr;
		tnapi->last_tx_cons = tnapi->tx_cons;
	}
}
