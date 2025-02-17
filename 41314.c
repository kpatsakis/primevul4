static void tg3_dump_state(struct tg3 *tp)
{
	int i;
	u32 *regs;

	regs = kzalloc(TG3_REG_BLK_SIZE, GFP_ATOMIC);
	if (!regs)
		return;

	if (tg3_flag(tp, PCI_EXPRESS)) {
		/* Read up to but not including private PCI registers */
		for (i = 0; i < TG3_PCIE_TLDLPL_PORT; i += sizeof(u32))
			regs[i / sizeof(u32)] = tr32(i);
	} else
		tg3_dump_legacy_regs(tp, regs);

	for (i = 0; i < TG3_REG_BLK_SIZE / sizeof(u32); i += 4) {
		if (!regs[i + 0] && !regs[i + 1] &&
		    !regs[i + 2] && !regs[i + 3])
			continue;

		netdev_err(tp->dev, "0x%08x: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
			   i * 4,
			   regs[i + 0], regs[i + 1], regs[i + 2], regs[i + 3]);
	}

	kfree(regs);

	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		/* SW status block */
		netdev_err(tp->dev,
			 "%d: Host status block [%08x:%08x:(%04x:%04x:%04x):(%04x:%04x)]\n",
			   i,
			   tnapi->hw_status->status,
			   tnapi->hw_status->status_tag,
			   tnapi->hw_status->rx_jumbo_consumer,
			   tnapi->hw_status->rx_consumer,
			   tnapi->hw_status->rx_mini_consumer,
			   tnapi->hw_status->idx[0].rx_producer,
			   tnapi->hw_status->idx[0].tx_consumer);

		netdev_err(tp->dev,
		"%d: NAPI info [%08x:%08x:(%04x:%04x:%04x):%04x:(%04x:%04x:%04x:%04x)]\n",
			   i,
			   tnapi->last_tag, tnapi->last_irq_tag,
			   tnapi->tx_prod, tnapi->tx_cons, tnapi->tx_pending,
			   tnapi->rx_rcb_ptr,
			   tnapi->prodring.rx_std_prod_idx,
			   tnapi->prodring.rx_std_cons_idx,
			   tnapi->prodring.rx_jmb_prod_idx,
			   tnapi->prodring.rx_jmb_cons_idx);
	}
}
