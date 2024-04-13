static void catc_stats_done(struct catc *catc, struct ctrl_queue *q)
{
	int index = q->index - EthStats;
	u16 data, last;

	catc->stats_buf[index] = *((char *)q->buf);

	if (index & 1)
		return;

	data = ((u16)catc->stats_buf[index] << 8) | catc->stats_buf[index + 1];
	last = catc->stats_vals[index >> 1];

	switch (index) {
		case TxSingleColl:
		case TxMultiColl:
			catc->netdev->stats.collisions += data - last;
			break;
		case TxExcessColl:
			catc->netdev->stats.tx_aborted_errors += data - last;
			catc->netdev->stats.tx_errors += data - last;
			break;
		case RxFramErr:
			catc->netdev->stats.rx_frame_errors += data - last;
			catc->netdev->stats.rx_errors += data - last;
			break;
	}

	catc->stats_vals[index >> 1] = data;
}
