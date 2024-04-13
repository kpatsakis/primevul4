fst_disable_intr(struct fst_card_info *card)
{
	if (card->family == FST_FAMILY_TXU) {
		outl(0x00000000, card->pci_conf + INTCSR_9054);
	} else {
		outw(0x0000, card->pci_conf + INTCSR_9052);
	}
}
