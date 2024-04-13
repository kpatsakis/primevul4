static int tg3_start(struct tg3 *tp, bool reset_phy, bool test_irq,
		     bool init)
{
	struct net_device *dev = tp->dev;
	int i, err;

	/*
	 * Setup interrupts first so we know how
	 * many NAPI resources to allocate
	 */
	tg3_ints_init(tp);

	tg3_rss_check_indir_tbl(tp);

	/* The placement of this call is tied
	 * to the setup and use of Host TX descriptors.
	 */
	err = tg3_alloc_consistent(tp);
	if (err)
		goto err_out1;

	tg3_napi_init(tp);

	tg3_napi_enable(tp);

	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];
		err = tg3_request_irq(tp, i);
		if (err) {
			for (i--; i >= 0; i--) {
				tnapi = &tp->napi[i];
				free_irq(tnapi->irq_vec, tnapi);
			}
			goto err_out2;
		}
	}

	tg3_full_lock(tp, 0);

	err = tg3_init_hw(tp, reset_phy);
	if (err) {
		tg3_halt(tp, RESET_KIND_SHUTDOWN, 1);
		tg3_free_rings(tp);
	}

	tg3_full_unlock(tp);

	if (err)
		goto err_out3;

	if (test_irq && tg3_flag(tp, USING_MSI)) {
		err = tg3_test_msi(tp);

		if (err) {
			tg3_full_lock(tp, 0);
			tg3_halt(tp, RESET_KIND_SHUTDOWN, 1);
			tg3_free_rings(tp);
			tg3_full_unlock(tp);

			goto err_out2;
		}

		if (!tg3_flag(tp, 57765_PLUS) && tg3_flag(tp, USING_MSI)) {
			u32 val = tr32(PCIE_TRANSACTION_CFG);

			tw32(PCIE_TRANSACTION_CFG,
			     val | PCIE_TRANS_CFG_1SHOT_MSI);
		}
	}

	tg3_phy_start(tp);

	tg3_hwmon_open(tp);

	tg3_full_lock(tp, 0);

	tg3_timer_start(tp);
	tg3_flag_set(tp, INIT_COMPLETE);
	tg3_enable_ints(tp);

	if (init)
		tg3_ptp_init(tp);
	else
		tg3_ptp_resume(tp);


	tg3_full_unlock(tp);

	netif_tx_start_all_queues(dev);

	/*
	 * Reset loopback feature if it was turned on while the device was down
	 * make sure that it's installed properly now.
	 */
	if (dev->features & NETIF_F_LOOPBACK)
		tg3_set_loopback(dev, dev->features);

	return 0;

err_out3:
	for (i = tp->irq_cnt - 1; i >= 0; i--) {
		struct tg3_napi *tnapi = &tp->napi[i];
		free_irq(tnapi->irq_vec, tnapi);
	}

err_out2:
	tg3_napi_disable(tp);
	tg3_napi_fini(tp);
	tg3_free_consistent(tp);

err_out1:
	tg3_ints_fini(tp);

	return err;
}
