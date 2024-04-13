fst_tx_dma(struct fst_card_info *card, unsigned char *skb,
	   unsigned char *mem, int len)
{
	/*
	 * This routine will setup the DMA and start it.
	 */

	dbg(DBG_TX, "In fst_tx_dma %p %p %d\n", skb, mem, len);
	if (card->dmatx_in_progress) {
		dbg(DBG_ASS, "In fst_tx_dma while dma in progress\n");
	}

	outl((unsigned long) skb, card->pci_conf + DMAPADR1);	/* Copy from here */
	outl((unsigned long) mem, card->pci_conf + DMALADR1);	/* to here */
	outl(len, card->pci_conf + DMASIZ1);	/* for this length */
	outl(0x000000004, card->pci_conf + DMADPR1);	/* In this direction */

	/*
	 * We use the dmatx_in_progress to flag the channel as busy
	 */
	card->dmatx_in_progress = 1;
	outb(0x03, card->pci_conf + DMACSR1);	/* Start the transfer */
}
