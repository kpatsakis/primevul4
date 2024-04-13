do_bottom_half_rx(struct fst_card_info *card)
{
	struct fst_port_info *port;
	int pi;
	int rx_count = 0;

	/* Check for rx completions on all ports on this card */
	dbg(DBG_RX, "do_bottom_half_rx\n");
	for (pi = 0, port = card->ports; pi < card->nports; pi++, port++) {
		if (!port->run)
			continue;

		while (!(FST_RDB(card, rxDescrRing[pi][port->rxpos].bits)
			 & DMA_OWN) && !(card->dmarx_in_progress)) {
			if (rx_count > fst_max_reads) {
				/*
				 * Don't spend forever in receive processing
				 * Schedule another event
				 */
				fst_q_work_item(&fst_work_intq, card->card_no);
				tasklet_schedule(&fst_int_task);
				break;	/* Leave the loop */
			}
			fst_intr_rx(card, port);
			rx_count++;
		}
	}
}
