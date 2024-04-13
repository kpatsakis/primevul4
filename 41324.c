static void tg3_frob_aux_power(struct tg3 *tp, bool include_wol)
{
	bool need_vaux = false;

	/* The GPIOs do something completely different on 57765. */
	if (!tg3_flag(tp, IS_NIC) || tg3_flag(tp, 57765_CLASS))
		return;

	if (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	    tg3_asic_rev(tp) == ASIC_REV_5719 ||
	    tg3_asic_rev(tp) == ASIC_REV_5720) {
		tg3_frob_aux_power_5717(tp, include_wol ?
					tg3_flag(tp, WOL_ENABLE) != 0 : 0);
		return;
	}

	if (tp->pdev_peer && tp->pdev_peer != tp->pdev) {
		struct net_device *dev_peer;

		dev_peer = pci_get_drvdata(tp->pdev_peer);

		/* remove_one() may have been run on the peer. */
		if (dev_peer) {
			struct tg3 *tp_peer = netdev_priv(dev_peer);

			if (tg3_flag(tp_peer, INIT_COMPLETE))
				return;

			if ((include_wol && tg3_flag(tp_peer, WOL_ENABLE)) ||
			    tg3_flag(tp_peer, ENABLE_ASF))
				need_vaux = true;
		}
	}

	if ((include_wol && tg3_flag(tp, WOL_ENABLE)) ||
	    tg3_flag(tp, ENABLE_ASF))
		need_vaux = true;

	if (need_vaux)
		tg3_pwrsrc_switch_to_vaux(tp);
	else
		tg3_pwrsrc_die_with_vmain(tp);
}
