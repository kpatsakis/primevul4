fst_init_card(struct fst_card_info *card)
{
	int i;
	int err;

	/* We're working on a number of ports based on the card ID. If the
	 * firmware detects something different later (should never happen)
	 * we'll have to revise it in some way then.
	 */
	for (i = 0; i < card->nports; i++) {
                err = register_hdlc_device(card->ports[i].dev);
                if (err < 0) {
			int j;
			pr_err("Cannot register HDLC device for port %d (errno %d)\n",
			       i, -err);
			for (j = i; j < card->nports; j++) {
				free_netdev(card->ports[j].dev);
				card->ports[j].dev = NULL;
			}
                        card->nports = i;
                        break;
                }
	}

	pr_info("%s-%s: %s IRQ%d, %d ports\n",
		port_to_dev(&card->ports[0])->name,
		port_to_dev(&card->ports[card->nports - 1])->name,
		type_strings[card->type], card->irq, card->nports);
}
