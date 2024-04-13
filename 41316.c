static bool tg3_enable_msix(struct tg3 *tp)
{
	int i, rc;
	struct msix_entry msix_ent[TG3_IRQ_MAX_VECS];

	tp->txq_cnt = tp->txq_req;
	tp->rxq_cnt = tp->rxq_req;
	if (!tp->rxq_cnt)
		tp->rxq_cnt = netif_get_num_default_rss_queues();
	if (tp->rxq_cnt > tp->rxq_max)
		tp->rxq_cnt = tp->rxq_max;

	/* Disable multiple TX rings by default.  Simple round-robin hardware
	 * scheduling of the TX rings can cause starvation of rings with
	 * small packets when other rings have TSO or jumbo packets.
	 */
	if (!tp->txq_req)
		tp->txq_cnt = 1;

	tp->irq_cnt = tg3_irq_count(tp);

	for (i = 0; i < tp->irq_max; i++) {
		msix_ent[i].entry  = i;
		msix_ent[i].vector = 0;
	}

	rc = pci_enable_msix(tp->pdev, msix_ent, tp->irq_cnt);
	if (rc < 0) {
		return false;
	} else if (rc != 0) {
		if (pci_enable_msix(tp->pdev, msix_ent, rc))
			return false;
		netdev_notice(tp->dev, "Requested %d MSI-X vectors, received %d\n",
			      tp->irq_cnt, rc);
		tp->irq_cnt = rc;
		tp->rxq_cnt = max(rc - 1, 1);
		if (tp->txq_cnt)
			tp->txq_cnt = min(tp->rxq_cnt, tp->txq_max);
	}

	for (i = 0; i < tp->irq_max; i++)
		tp->napi[i].irq_vec = msix_ent[i].vector;

	if (netif_set_real_num_rx_queues(tp->dev, tp->rxq_cnt)) {
		pci_disable_msix(tp->pdev);
		return false;
	}

	if (tp->irq_cnt == 1)
		return true;

	tg3_flag_set(tp, ENABLE_RSS);

	if (tp->txq_cnt > 1)
		tg3_flag_set(tp, ENABLE_TSS);

	netif_set_real_num_tx_queues(tp->dev, tp->txq_cnt);

	return true;
}
