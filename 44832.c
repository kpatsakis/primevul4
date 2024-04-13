fst_init_dma(struct fst_card_info *card)
{
	/*
	 * This is only required for the PLX 9054
	 */
	if (card->family == FST_FAMILY_TXU) {
	        pci_set_master(card->device);
		outl(0x00020441, card->pci_conf + DMAMODE0);
		outl(0x00020441, card->pci_conf + DMAMODE1);
		outl(0x0, card->pci_conf + DMATHR);
	}
}
