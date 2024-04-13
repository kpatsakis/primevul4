fst_remove_one(struct pci_dev *pdev)
{
	struct fst_card_info *card;
	int i;

	card = pci_get_drvdata(pdev);

	for (i = 0; i < card->nports; i++) {
		struct net_device *dev = port_to_dev(&card->ports[i]);
		unregister_hdlc_device(dev);
	}

	fst_disable_intr(card);
	free_irq(card->irq, card);

	iounmap(card->ctlmem);
	iounmap(card->mem);
	pci_release_regions(pdev);
	if (card->family == FST_FAMILY_TXU) {
		/*
		 * Free dma buffers
		 */
		pci_free_consistent(card->device, FST_MAX_MTU,
				    card->rx_dma_handle_host,
				    card->rx_dma_handle_card);
		pci_free_consistent(card->device, FST_MAX_MTU,
				    card->tx_dma_handle_host,
				    card->tx_dma_handle_card);
	}
	fst_card_array[card->card_no] = NULL;
}
