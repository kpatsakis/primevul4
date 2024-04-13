fst_log_rx_error(struct fst_card_info *card, struct fst_port_info *port,
		 unsigned char dmabits, int rxp, unsigned short len)
{
	struct net_device *dev = port_to_dev(port);

	/*
	 * Increment the appropriate error counter
	 */
	dev->stats.rx_errors++;
	if (dmabits & RX_OFLO) {
		dev->stats.rx_fifo_errors++;
		dbg(DBG_ASS, "Rx fifo error on card %d port %d buffer %d\n",
		    card->card_no, port->index, rxp);
	}
	if (dmabits & RX_CRC) {
		dev->stats.rx_crc_errors++;
		dbg(DBG_ASS, "Rx crc error on card %d port %d\n",
		    card->card_no, port->index);
	}
	if (dmabits & RX_FRAM) {
		dev->stats.rx_frame_errors++;
		dbg(DBG_ASS, "Rx frame error on card %d port %d\n",
		    card->card_no, port->index);
	}
	if (dmabits == (RX_STP | RX_ENP)) {
		dev->stats.rx_length_errors++;
		dbg(DBG_ASS, "Rx length error (%d) on card %d port %d\n",
		    len, card->card_no, port->index);
	}
}
