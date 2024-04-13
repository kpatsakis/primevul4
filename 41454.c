static int tg3_poll_work(struct tg3_napi *tnapi, int work_done, int budget)
{
	struct tg3 *tp = tnapi->tp;

	/* run TX completion thread */
	if (tnapi->hw_status->idx[0].tx_consumer != tnapi->tx_cons) {
		tg3_tx(tnapi);
		if (unlikely(tg3_flag(tp, TX_RECOVERY_PENDING)))
			return work_done;
	}

	if (!tnapi->rx_rcb_prod_idx)
		return work_done;

	/* run RX thread, within the bounds set by NAPI.
	 * All RX "locking" is done by ensuring outside
	 * code synchronizes with tg3->napi.poll()
	 */
	if (*(tnapi->rx_rcb_prod_idx) != tnapi->rx_rcb_ptr)
		work_done += tg3_rx(tnapi, budget - work_done);

	if (tg3_flag(tp, ENABLE_RSS) && tnapi == &tp->napi[1]) {
		struct tg3_rx_prodring_set *dpr = &tp->napi[0].prodring;
		int i, err = 0;
		u32 std_prod_idx = dpr->rx_std_prod_idx;
		u32 jmb_prod_idx = dpr->rx_jmb_prod_idx;

		tp->rx_refill = false;
		for (i = 1; i <= tp->rxq_cnt; i++)
			err |= tg3_rx_prodring_xfer(tp, dpr,
						    &tp->napi[i].prodring);

		wmb();

		if (std_prod_idx != dpr->rx_std_prod_idx)
			tw32_rx_mbox(TG3_RX_STD_PROD_IDX_REG,
				     dpr->rx_std_prod_idx);

		if (jmb_prod_idx != dpr->rx_jmb_prod_idx)
			tw32_rx_mbox(TG3_RX_JMB_PROD_IDX_REG,
				     dpr->rx_jmb_prod_idx);

		mmiowb();

		if (err)
			tw32_f(HOSTCC_MODE, tp->coal_now);
	}

	return work_done;
}
