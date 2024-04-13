static int tg3_open(struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);
	int err;

	if (tp->fw_needed) {
		err = tg3_request_firmware(tp);
		if (tg3_chip_rev_id(tp) == CHIPREV_ID_5701_A0) {
			if (err)
				return err;
		} else if (err) {
			netdev_warn(tp->dev, "TSO capability disabled\n");
			tg3_flag_clear(tp, TSO_CAPABLE);
		} else if (!tg3_flag(tp, TSO_CAPABLE)) {
			netdev_notice(tp->dev, "TSO capability restored\n");
			tg3_flag_set(tp, TSO_CAPABLE);
		}
	}

	tg3_carrier_off(tp);

	err = tg3_power_up(tp);
	if (err)
		return err;

	tg3_full_lock(tp, 0);

	tg3_disable_ints(tp);
	tg3_flag_clear(tp, INIT_COMPLETE);

	tg3_full_unlock(tp);

	err = tg3_start(tp, true, true, true);
	if (err) {
		tg3_frob_aux_power(tp, false);
		pci_set_power_state(tp->pdev, PCI_D3hot);
	}

	if (tg3_flag(tp, PTP_CAPABLE)) {
		tp->ptp_clock = ptp_clock_register(&tp->ptp_info,
						   &tp->pdev->dev);
		if (IS_ERR(tp->ptp_clock))
			tp->ptp_clock = NULL;
	}

	return err;
}
