static void tg3_ints_init(struct tg3 *tp)
{
	if ((tg3_flag(tp, SUPPORT_MSI) || tg3_flag(tp, SUPPORT_MSIX)) &&
	    !tg3_flag(tp, TAGGED_STATUS)) {
		/* All MSI supporting chips should support tagged
		 * status.  Assert that this is the case.
		 */
		netdev_warn(tp->dev,
			    "MSI without TAGGED_STATUS? Not using MSI\n");
		goto defcfg;
	}

	if (tg3_flag(tp, SUPPORT_MSIX) && tg3_enable_msix(tp))
		tg3_flag_set(tp, USING_MSIX);
	else if (tg3_flag(tp, SUPPORT_MSI) && pci_enable_msi(tp->pdev) == 0)
		tg3_flag_set(tp, USING_MSI);

	if (tg3_flag(tp, USING_MSI) || tg3_flag(tp, USING_MSIX)) {
		u32 msi_mode = tr32(MSGINT_MODE);
		if (tg3_flag(tp, USING_MSIX) && tp->irq_cnt > 1)
			msi_mode |= MSGINT_MODE_MULTIVEC_EN;
		if (!tg3_flag(tp, 1SHOT_MSI))
			msi_mode |= MSGINT_MODE_ONE_SHOT_DISABLE;
		tw32(MSGINT_MODE, msi_mode | MSGINT_MODE_ENABLE);
	}
defcfg:
	if (!tg3_flag(tp, USING_MSIX)) {
		tp->irq_cnt = 1;
		tp->napi[0].irq_vec = tp->pdev->irq;
	}

	if (tp->irq_cnt == 1) {
		tp->txq_cnt = 1;
		tp->rxq_cnt = 1;
		netif_set_real_num_tx_queues(tp->dev, 1);
		netif_set_real_num_rx_queues(tp->dev, 1);
	}
}
