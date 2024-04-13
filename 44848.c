fst_recover_rx_error(struct fst_card_info *card, struct fst_port_info *port,
		     unsigned char dmabits, int rxp, unsigned short len)
{
	int i;
	int pi;

	pi = port->index;
	/* 
	 * Discard buffer descriptors until we see the start of the
	 * next frame.  Note that for long frames this could be in
	 * a subsequent interrupt. 
	 */
	i = 0;
	while ((dmabits & (DMA_OWN | RX_STP)) == 0) {
		FST_WRB(card, rxDescrRing[pi][rxp].bits, DMA_OWN);
		rxp = (rxp+1) % NUM_RX_BUFFER;
		if (++i > NUM_RX_BUFFER) {
			dbg(DBG_ASS, "intr_rx: Discarding more bufs"
			    " than we have\n");
			break;
		}
		dmabits = FST_RDB(card, rxDescrRing[pi][rxp].bits);
		dbg(DBG_ASS, "DMA Bits of next buffer was %x\n", dmabits);
	}
	dbg(DBG_ASS, "There were %d subsequent buffers in error\n", i);

	/* Discard the terminal buffer */
	if (!(dmabits & DMA_OWN)) {
		FST_WRB(card, rxDescrRing[pi][rxp].bits, DMA_OWN);
		rxp = (rxp+1) % NUM_RX_BUFFER;
	}
	port->rxpos = rxp;
	return;

}
