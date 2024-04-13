static inline unsigned int tg3_has_work(struct tg3_napi *tnapi)
{
	struct tg3 *tp = tnapi->tp;
	struct tg3_hw_status *sblk = tnapi->hw_status;
	unsigned int work_exists = 0;

	/* check for phy events */
	if (!(tg3_flag(tp, USE_LINKCHG_REG) || tg3_flag(tp, POLL_SERDES))) {
		if (sblk->status & SD_STATUS_LINK_CHG)
			work_exists = 1;
	}

	/* check for TX work to do */
	if (sblk->idx[0].tx_consumer != tnapi->tx_cons)
		work_exists = 1;

	/* check for RX work to do */
	if (tnapi->rx_rcb_prod_idx &&
	    *(tnapi->rx_rcb_prod_idx) != tnapi->rx_rcb_ptr)
		work_exists = 1;

	return work_exists;
}
