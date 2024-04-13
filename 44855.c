fst_tx_config(struct fst_port_info *port)
{
	int i;
	int pi;
	unsigned int offset;
	unsigned long flags;
	struct fst_card_info *card;

	pi = port->index;
	card = port->card;
	spin_lock_irqsave(&card->card_lock, flags);
	for (i = 0; i < NUM_TX_BUFFER; i++) {
		offset = BUF_OFFSET(txBuffer[pi][i][0]);

		FST_WRW(card, txDescrRing[pi][i].ladr, (u16) offset);
		FST_WRB(card, txDescrRing[pi][i].hadr, (u8) (offset >> 16));
		FST_WRW(card, txDescrRing[pi][i].bcnt, 0);
		FST_WRB(card, txDescrRing[pi][i].bits, 0);
	}
	port->txpos = 0;
	port->txipos = 0;
	port->start = 0;
	spin_unlock_irqrestore(&card->card_lock, flags);
}
