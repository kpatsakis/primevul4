static int tg3_test_loopback(struct tg3 *tp, u64 *data, bool do_extlpbk)
{
	int err = -EIO;
	u32 eee_cap;
	u32 jmb_pkt_sz = 9000;

	if (tp->dma_limit)
		jmb_pkt_sz = tp->dma_limit - ETH_HLEN;

	eee_cap = tp->phy_flags & TG3_PHYFLG_EEE_CAP;
	tp->phy_flags &= ~TG3_PHYFLG_EEE_CAP;

	if (!netif_running(tp->dev)) {
		data[TG3_MAC_LOOPB_TEST] = TG3_LOOPBACK_FAILED;
		data[TG3_PHY_LOOPB_TEST] = TG3_LOOPBACK_FAILED;
		if (do_extlpbk)
			data[TG3_EXT_LOOPB_TEST] = TG3_LOOPBACK_FAILED;
		goto done;
	}

	err = tg3_reset_hw(tp, 1);
	if (err) {
		data[TG3_MAC_LOOPB_TEST] = TG3_LOOPBACK_FAILED;
		data[TG3_PHY_LOOPB_TEST] = TG3_LOOPBACK_FAILED;
		if (do_extlpbk)
			data[TG3_EXT_LOOPB_TEST] = TG3_LOOPBACK_FAILED;
		goto done;
	}

	if (tg3_flag(tp, ENABLE_RSS)) {
		int i;

		/* Reroute all rx packets to the 1st queue */
		for (i = MAC_RSS_INDIR_TBL_0;
		     i < MAC_RSS_INDIR_TBL_0 + TG3_RSS_INDIR_TBL_SIZE; i += 4)
			tw32(i, 0x0);
	}

	/* HW errata - mac loopback fails in some cases on 5780.
	 * Normal traffic and PHY loopback are not affected by
	 * errata.  Also, the MAC loopback test is deprecated for
	 * all newer ASIC revisions.
	 */
	if (tg3_asic_rev(tp) != ASIC_REV_5780 &&
	    !tg3_flag(tp, CPMU_PRESENT)) {
		tg3_mac_loopback(tp, true);

		if (tg3_run_loopback(tp, ETH_FRAME_LEN, false))
			data[TG3_MAC_LOOPB_TEST] |= TG3_STD_LOOPBACK_FAILED;

		if (tg3_flag(tp, JUMBO_RING_ENABLE) &&
		    tg3_run_loopback(tp, jmb_pkt_sz + ETH_HLEN, false))
			data[TG3_MAC_LOOPB_TEST] |= TG3_JMB_LOOPBACK_FAILED;

		tg3_mac_loopback(tp, false);
	}

	if (!(tp->phy_flags & TG3_PHYFLG_PHY_SERDES) &&
	    !tg3_flag(tp, USE_PHYLIB)) {
		int i;

		tg3_phy_lpbk_set(tp, 0, false);

		/* Wait for link */
		for (i = 0; i < 100; i++) {
			if (tr32(MAC_TX_STATUS) & TX_STATUS_LINK_UP)
				break;
			mdelay(1);
		}

		if (tg3_run_loopback(tp, ETH_FRAME_LEN, false))
			data[TG3_PHY_LOOPB_TEST] |= TG3_STD_LOOPBACK_FAILED;
		if (tg3_flag(tp, TSO_CAPABLE) &&
		    tg3_run_loopback(tp, ETH_FRAME_LEN, true))
			data[TG3_PHY_LOOPB_TEST] |= TG3_TSO_LOOPBACK_FAILED;
		if (tg3_flag(tp, JUMBO_RING_ENABLE) &&
		    tg3_run_loopback(tp, jmb_pkt_sz + ETH_HLEN, false))
			data[TG3_PHY_LOOPB_TEST] |= TG3_JMB_LOOPBACK_FAILED;

		if (do_extlpbk) {
			tg3_phy_lpbk_set(tp, 0, true);

			/* All link indications report up, but the hardware
			 * isn't really ready for about 20 msec.  Double it
			 * to be sure.
			 */
			mdelay(40);

			if (tg3_run_loopback(tp, ETH_FRAME_LEN, false))
				data[TG3_EXT_LOOPB_TEST] |=
							TG3_STD_LOOPBACK_FAILED;
			if (tg3_flag(tp, TSO_CAPABLE) &&
			    tg3_run_loopback(tp, ETH_FRAME_LEN, true))
				data[TG3_EXT_LOOPB_TEST] |=
							TG3_TSO_LOOPBACK_FAILED;
			if (tg3_flag(tp, JUMBO_RING_ENABLE) &&
			    tg3_run_loopback(tp, jmb_pkt_sz + ETH_HLEN, false))
				data[TG3_EXT_LOOPB_TEST] |=
							TG3_JMB_LOOPBACK_FAILED;
		}

		/* Re-enable gphy autopowerdown. */
		if (tp->phy_flags & TG3_PHYFLG_ENABLE_APD)
			tg3_phy_toggle_apd(tp, true);
	}

	err = (data[TG3_MAC_LOOPB_TEST] | data[TG3_PHY_LOOPB_TEST] |
	       data[TG3_EXT_LOOPB_TEST]) ? -EIO : 0;

done:
	tp->phy_flags |= eee_cap;

	return err;
}
