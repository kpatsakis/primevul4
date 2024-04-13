fst_rx_config(struct fst_port_info *port)
{
	int i;
	int pi;
	unsigned int offset;
	unsigned long flags;
	struct fst_card_info *card;

	pi = port->index;
	card = port->card;
	spin_lock_irqsave(&card->card_lock, flags);
	for (i = 0; i < NUM_RX_BUFFER; i++) {
		offset = BUF_OFFSET(rxBuffer[pi][i][0]);

		FST_WRW(card, rxDescrRing[pi][i].ladr, (u16) offset);
		FST_WRB(card, rxDescrRing[pi][i].hadr, (u8) (offset >> 16));
		FST_WRW(card, rxDescrRing[pi][i].bcnt, cnv_bcnt(LEN_RX_BUFFER));
		FST_WRW(card, rxDescrRing[pi][i].mcnt, LEN_RX_BUFFER);
		FST_WRB(card, rxDescrRing[pi][i].bits, DMA_OWN);
	}
	port->rxpos = 0;
	spin_unlock_irqrestore(&card->card_lock, flags);
}
