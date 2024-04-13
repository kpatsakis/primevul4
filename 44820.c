fst_add_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	static int no_of_cards_added = 0;
	struct fst_card_info *card;
	int err = 0;
	int i;

	printk_once(KERN_INFO
		    pr_fmt("FarSync WAN driver " FST_USER_VERSION
			   " (c) 2001-2004 FarSite Communications Ltd.\n"));
#if FST_DEBUG
	dbg(DBG_ASS, "The value of debug mask is %x\n", fst_debug_mask);
#endif
	/*
	 * We are going to be clever and allow certain cards not to be
	 * configured.  An exclude list can be provided in /etc/modules.conf
	 */
	if (fst_excluded_cards != 0) {
		/*
		 * There are cards to exclude
		 *
		 */
		for (i = 0; i < fst_excluded_cards; i++) {
			if ((pdev->devfn) >> 3 == fst_excluded_list[i]) {
				pr_info("FarSync PCI device %d not assigned\n",
					(pdev->devfn) >> 3);
				return -EBUSY;
			}
		}
	}

	/* Allocate driver private data */
	card = kzalloc(sizeof(struct fst_card_info), GFP_KERNEL);
	if (card == NULL)
		return -ENOMEM;

	/* Try to enable the device */
	if ((err = pci_enable_device(pdev)) != 0) {
		pr_err("Failed to enable card. Err %d\n", -err);
		kfree(card);
		return err;
	}

	if ((err = pci_request_regions(pdev, "FarSync")) !=0) {
		pr_err("Failed to allocate regions. Err %d\n", -err);
		pci_disable_device(pdev);
		kfree(card);
	        return err;
	}

	/* Get virtual addresses of memory regions */
	card->pci_conf = pci_resource_start(pdev, 1);
	card->phys_mem = pci_resource_start(pdev, 2);
	card->phys_ctlmem = pci_resource_start(pdev, 3);
	if ((card->mem = ioremap(card->phys_mem, FST_MEMSIZE)) == NULL) {
		pr_err("Physical memory remap failed\n");
		pci_release_regions(pdev);
		pci_disable_device(pdev);
		kfree(card);
		return -ENODEV;
	}
	if ((card->ctlmem = ioremap(card->phys_ctlmem, 0x10)) == NULL) {
		pr_err("Control memory remap failed\n");
		pci_release_regions(pdev);
		pci_disable_device(pdev);
		iounmap(card->mem);
		kfree(card);
		return -ENODEV;
	}
	dbg(DBG_PCI, "kernel mem %p, ctlmem %p\n", card->mem, card->ctlmem);

	/* Register the interrupt handler */
	if (request_irq(pdev->irq, fst_intr, IRQF_SHARED, FST_DEV_NAME, card)) {
		pr_err("Unable to register interrupt %d\n", card->irq);
		pci_release_regions(pdev);
		pci_disable_device(pdev);
		iounmap(card->ctlmem);
		iounmap(card->mem);
		kfree(card);
		return -ENODEV;
	}

	/* Record info we need */
	card->irq = pdev->irq;
	card->type = ent->driver_data;
	card->family = ((ent->driver_data == FST_TYPE_T2P) ||
			(ent->driver_data == FST_TYPE_T4P))
	    ? FST_FAMILY_TXP : FST_FAMILY_TXU;
	if ((ent->driver_data == FST_TYPE_T1U) ||
	    (ent->driver_data == FST_TYPE_TE1))
		card->nports = 1;
	else
		card->nports = ((ent->driver_data == FST_TYPE_T2P) ||
				(ent->driver_data == FST_TYPE_T2U)) ? 2 : 4;

	card->state = FST_UNINIT;
        spin_lock_init ( &card->card_lock );

        for ( i = 0 ; i < card->nports ; i++ ) {
		struct net_device *dev = alloc_hdlcdev(&card->ports[i]);
		hdlc_device *hdlc;
		if (!dev) {
			while (i--)
				free_netdev(card->ports[i].dev);
			pr_err("FarSync: out of memory\n");
                        free_irq(card->irq, card);
                        pci_release_regions(pdev);
                        pci_disable_device(pdev);
                        iounmap(card->ctlmem);
                        iounmap(card->mem);
                        kfree(card);
                        return -ENODEV;
		}
		card->ports[i].dev    = dev;
                card->ports[i].card   = card;
                card->ports[i].index  = i;
                card->ports[i].run    = 0;

		hdlc = dev_to_hdlc(dev);

                /* Fill in the net device info */
		/* Since this is a PCI setup this is purely
		 * informational. Give them the buffer addresses
		 * and basic card I/O.
		 */
                dev->mem_start   = card->phys_mem
                                 + BUF_OFFSET ( txBuffer[i][0][0]);
                dev->mem_end     = card->phys_mem
                                 + BUF_OFFSET ( txBuffer[i][NUM_TX_BUFFER][0]);
                dev->base_addr   = card->pci_conf;
                dev->irq         = card->irq;

		dev->netdev_ops = &fst_ops;
		dev->tx_queue_len = FST_TX_QUEUE_LEN;
		dev->watchdog_timeo = FST_TX_TIMEOUT;
                hdlc->attach = fst_attach;
                hdlc->xmit   = fst_start_xmit;
	}

	card->device = pdev;

	dbg(DBG_PCI, "type %d nports %d irq %d\n", card->type,
	    card->nports, card->irq);
	dbg(DBG_PCI, "conf %04x mem %08x ctlmem %08x\n",
	    card->pci_conf, card->phys_mem, card->phys_ctlmem);

	/* Reset the card's processor */
	fst_cpureset(card);
	card->state = FST_RESET;

	/* Initialise DMA (if required) */
	fst_init_dma(card);

	/* Record driver data for later use */
	pci_set_drvdata(pdev, card);

	/* Remainder of card setup */
	fst_card_array[no_of_cards_added] = card;
	card->card_no = no_of_cards_added++;	/* Record instance and bump it */
	fst_init_card(card);
	if (card->family == FST_FAMILY_TXU) {
		/*
		 * Allocate a dma buffer for transmit and receives
		 */
		card->rx_dma_handle_host =
		    pci_alloc_consistent(card->device, FST_MAX_MTU,
					 &card->rx_dma_handle_card);
		if (card->rx_dma_handle_host == NULL) {
			pr_err("Could not allocate rx dma buffer\n");
			fst_disable_intr(card);
			pci_release_regions(pdev);
			pci_disable_device(pdev);
			iounmap(card->ctlmem);
			iounmap(card->mem);
			kfree(card);
			return -ENOMEM;
		}
		card->tx_dma_handle_host =
		    pci_alloc_consistent(card->device, FST_MAX_MTU,
					 &card->tx_dma_handle_card);
		if (card->tx_dma_handle_host == NULL) {
			pr_err("Could not allocate tx dma buffer\n");
			fst_disable_intr(card);
			pci_release_regions(pdev);
			pci_disable_device(pdev);
			iounmap(card->ctlmem);
			iounmap(card->mem);
			kfree(card);
			return -ENOMEM;
		}
	}
	return 0;		/* Success */
}
