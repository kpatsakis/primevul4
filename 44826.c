fst_cpurelease(struct fst_card_info *card)
{
	if (card->family == FST_FAMILY_TXU) {
		/*
		 * Force posted writes to complete
		 */
		(void) readb(card->mem);

		/*
		 * Release LRESET DO = 1
		 * Then release Local Hold, DO = 1
		 */
		outw(0x040e, card->pci_conf + CNTRL_9054 + 2);
		outw(0x040f, card->pci_conf + CNTRL_9054 + 2);
	} else {
		(void) readb(card->ctlmem);
	}
}
