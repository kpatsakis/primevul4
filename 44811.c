static int wanxl_pci_init_one(struct pci_dev *pdev,
			      const struct pci_device_id *ent)
{
	card_t *card;
	u32 ramsize, stat;
	unsigned long timeout;
	u32 plx_phy;		/* PLX PCI base address */
	u32 mem_phy;		/* memory PCI base addr */
	u8 __iomem *mem;	/* memory virtual base addr */
	int i, ports, alloc_size;

#ifndef MODULE
	pr_info_once("%s\n", version);
#endif

	i = pci_enable_device(pdev);
	if (i)
		return i;

	/* QUICC can only access first 256 MB of host RAM directly,
	   but PLX9060 DMA does 32-bits for actual packet data transfers */

	/* FIXME when PCI/DMA subsystems are fixed.
	   We set both dma_mask and consistent_dma_mask to 28 bits
	   and pray pci_alloc_consistent() will use this info. It should
	   work on most platforms */
	if (pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(28)) ||
	    pci_set_dma_mask(pdev, DMA_BIT_MASK(28))) {
		pr_err("No usable DMA configuration\n");
		return -EIO;
	}

	i = pci_request_regions(pdev, "wanXL");
	if (i) {
		pci_disable_device(pdev);
		return i;
	}

	switch (pdev->device) {
	case PCI_DEVICE_ID_SBE_WANXL100: ports = 1; break;
	case PCI_DEVICE_ID_SBE_WANXL200: ports = 2; break;
	default: ports = 4;
	}

	alloc_size = sizeof(card_t) + ports * sizeof(port_t);
	card = kzalloc(alloc_size, GFP_KERNEL);
	if (card == NULL) {
		pci_release_regions(pdev);
		pci_disable_device(pdev);
		return -ENOBUFS;
	}

	pci_set_drvdata(pdev, card);
	card->pdev = pdev;

	card->status = pci_alloc_consistent(pdev, sizeof(card_status_t),
					    &card->status_address);
	if (card->status == NULL) {
		wanxl_pci_remove_one(pdev);
		return -ENOBUFS;
	}

#ifdef DEBUG_PCI
	printk(KERN_DEBUG "wanXL %s: pci_alloc_consistent() returned memory"
	       " at 0x%LX\n", pci_name(pdev),
	       (unsigned long long)card->status_address);
#endif

	/* FIXME when PCI/DMA subsystems are fixed.
	   We set both dma_mask and consistent_dma_mask back to 32 bits
	   to indicate the card can do 32-bit DMA addressing */
	if (pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32)) ||
	    pci_set_dma_mask(pdev, DMA_BIT_MASK(32))) {
		pr_err("No usable DMA configuration\n");
		wanxl_pci_remove_one(pdev);
		return -EIO;
	}

	/* set up PLX mapping */
	plx_phy = pci_resource_start(pdev, 0);

	card->plx = ioremap_nocache(plx_phy, 0x70);
	if (!card->plx) {
		pr_err("ioremap() failed\n");
 		wanxl_pci_remove_one(pdev);
		return -EFAULT;
	}

#if RESET_WHILE_LOADING
	wanxl_reset(card);
#endif

	timeout = jiffies + 20 * HZ;
	while ((stat = readl(card->plx + PLX_MAILBOX_0)) != 0) {
		if (time_before(timeout, jiffies)) {
			pr_warn("%s: timeout waiting for PUTS to complete\n",
				pci_name(pdev));
			wanxl_pci_remove_one(pdev);
			return -ENODEV;
		}

		switch(stat & 0xC0) {
		case 0x00:	/* hmm - PUTS completed with non-zero code? */
		case 0x80:	/* PUTS still testing the hardware */
			break;

		default:
			pr_warn("%s: PUTS test 0x%X failed\n",
				pci_name(pdev), stat & 0x30);
			wanxl_pci_remove_one(pdev);
			return -ENODEV;
		}

		schedule();
	}

	/* get on-board memory size (PUTS detects no more than 4 MB) */
	ramsize = readl(card->plx + PLX_MAILBOX_2) & MBX2_MEMSZ_MASK;

	/* set up on-board RAM mapping */
	mem_phy = pci_resource_start(pdev, 2);


	/* sanity check the board's reported memory size */
	if (ramsize < BUFFERS_ADDR +
	    (TX_BUFFERS + RX_BUFFERS) * BUFFER_LENGTH * ports) {
		pr_warn("%s: no enough on-board RAM (%u bytes detected, %u bytes required)\n",
			pci_name(pdev), ramsize,
			BUFFERS_ADDR +
			(TX_BUFFERS + RX_BUFFERS) * BUFFER_LENGTH * ports);
		wanxl_pci_remove_one(pdev);
		return -ENODEV;
	}

	if (wanxl_puts_command(card, MBX1_CMD_BSWAP)) {
		pr_warn("%s: unable to Set Byte Swap Mode\n", pci_name(pdev));
		wanxl_pci_remove_one(pdev);
		return -ENODEV;
	}

	for (i = 0; i < RX_QUEUE_LENGTH; i++) {
		struct sk_buff *skb = dev_alloc_skb(BUFFER_LENGTH);
		card->rx_skbs[i] = skb;
		if (skb)
			card->status->rx_descs[i].address =
				pci_map_single(card->pdev, skb->data,
					       BUFFER_LENGTH,
					       PCI_DMA_FROMDEVICE);
	}

	mem = ioremap_nocache(mem_phy, PDM_OFFSET + sizeof(firmware));
	if (!mem) {
		pr_err("ioremap() failed\n");
 		wanxl_pci_remove_one(pdev);
		return -EFAULT;
	}

	for (i = 0; i < sizeof(firmware); i += 4)
		writel(ntohl(*(__be32*)(firmware + i)), mem + PDM_OFFSET + i);

	for (i = 0; i < ports; i++)
		writel(card->status_address +
		       (void *)&card->status->port_status[i] -
		       (void *)card->status, mem + PDM_OFFSET + 4 + i * 4);
	writel(card->status_address, mem + PDM_OFFSET + 20);
	writel(PDM_OFFSET, mem);
	iounmap(mem);

	writel(0, card->plx + PLX_MAILBOX_5);

	if (wanxl_puts_command(card, MBX1_CMD_ABORTJ)) {
		pr_warn("%s: unable to Abort and Jump\n", pci_name(pdev));
		wanxl_pci_remove_one(pdev);
		return -ENODEV;
	}

	stat = 0;
	timeout = jiffies + 5 * HZ;
	do {
		if ((stat = readl(card->plx + PLX_MAILBOX_5)) != 0)
			break;
		schedule();
	}while (time_after(timeout, jiffies));

	if (!stat) {
		pr_warn("%s: timeout while initializing card firmware\n",
			pci_name(pdev));
		wanxl_pci_remove_one(pdev);
		return -ENODEV;
	}

#if DETECT_RAM
	ramsize = stat;
#endif

	pr_info("%s: at 0x%X, %u KB of RAM at 0x%X, irq %u\n",
		pci_name(pdev), plx_phy, ramsize / 1024, mem_phy, pdev->irq);

	/* Allocate IRQ */
	if (request_irq(pdev->irq, wanxl_intr, IRQF_SHARED, "wanXL", card)) {
		pr_warn("%s: could not allocate IRQ%i\n",
			pci_name(pdev), pdev->irq);
		wanxl_pci_remove_one(pdev);
		return -EBUSY;
	}
	card->irq = pdev->irq;

	for (i = 0; i < ports; i++) {
		hdlc_device *hdlc;
		port_t *port = &card->ports[i];
		struct net_device *dev = alloc_hdlcdev(port);
		if (!dev) {
			pr_err("%s: unable to allocate memory\n",
			       pci_name(pdev));
			wanxl_pci_remove_one(pdev);
			return -ENOMEM;
		}

		port->dev = dev;
		hdlc = dev_to_hdlc(dev);
		spin_lock_init(&port->lock);
		dev->tx_queue_len = 50;
		dev->netdev_ops = &wanxl_ops;
		hdlc->attach = wanxl_attach;
		hdlc->xmit = wanxl_xmit;
		port->card = card;
		port->node = i;
		get_status(port)->clocking = CLOCK_EXT;
		if (register_hdlc_device(dev)) {
			pr_err("%s: unable to register hdlc device\n",
			       pci_name(pdev));
			free_netdev(dev);
			wanxl_pci_remove_one(pdev);
			return -ENOBUFS;
		}
		card->n_ports++;
	}

	pr_info("%s: port", pci_name(pdev));
	for (i = 0; i < ports; i++)
		pr_cont("%s #%i: %s",
			i ? "," : "", i, card->ports[i].dev->name);
	pr_cont("\n");

	for (i = 0; i < ports; i++)
		wanxl_cable_intr(&card->ports[i]); /* get carrier status etc.*/

	return 0;
}
