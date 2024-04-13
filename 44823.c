fst_clear_intr(struct fst_card_info *card)
{
	if (card->family == FST_FAMILY_TXU) {
		(void) readb(card->ctlmem);
	} else {
		/* Poke the appropriate PLX chip register (same as enabling interrupts)
		 */
		outw(0x0543, card->pci_conf + INTCSR_9052);
	}
}
