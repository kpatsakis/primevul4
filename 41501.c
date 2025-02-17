static void tg3_self_test(struct net_device *dev, struct ethtool_test *etest,
			  u64 *data)
{
	struct tg3 *tp = netdev_priv(dev);
	bool doextlpbk = etest->flags & ETH_TEST_FL_EXTERNAL_LB;

	if ((tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER) &&
	    tg3_power_up(tp)) {
		etest->flags |= ETH_TEST_FL_FAILED;
		memset(data, 1, sizeof(u64) * TG3_NUM_TEST);
		return;
	}

	memset(data, 0, sizeof(u64) * TG3_NUM_TEST);

	if (tg3_test_nvram(tp) != 0) {
		etest->flags |= ETH_TEST_FL_FAILED;
		data[TG3_NVRAM_TEST] = 1;
	}
	if (!doextlpbk && tg3_test_link(tp)) {
		etest->flags |= ETH_TEST_FL_FAILED;
		data[TG3_LINK_TEST] = 1;
	}
	if (etest->flags & ETH_TEST_FL_OFFLINE) {
		int err, err2 = 0, irq_sync = 0;

		if (netif_running(dev)) {
			tg3_phy_stop(tp);
			tg3_netif_stop(tp);
			irq_sync = 1;
		}

		tg3_full_lock(tp, irq_sync);
		tg3_halt(tp, RESET_KIND_SUSPEND, 1);
		err = tg3_nvram_lock(tp);
		tg3_halt_cpu(tp, RX_CPU_BASE);
		if (!tg3_flag(tp, 5705_PLUS))
			tg3_halt_cpu(tp, TX_CPU_BASE);
		if (!err)
			tg3_nvram_unlock(tp);

		if (tp->phy_flags & TG3_PHYFLG_MII_SERDES)
			tg3_phy_reset(tp);

		if (tg3_test_registers(tp) != 0) {
			etest->flags |= ETH_TEST_FL_FAILED;
			data[TG3_REGISTER_TEST] = 1;
		}

		if (tg3_test_memory(tp) != 0) {
			etest->flags |= ETH_TEST_FL_FAILED;
			data[TG3_MEMORY_TEST] = 1;
		}

		if (doextlpbk)
			etest->flags |= ETH_TEST_FL_EXTERNAL_LB_DONE;

		if (tg3_test_loopback(tp, data, doextlpbk))
			etest->flags |= ETH_TEST_FL_FAILED;

		tg3_full_unlock(tp);

		if (tg3_test_interrupt(tp) != 0) {
			etest->flags |= ETH_TEST_FL_FAILED;
			data[TG3_INTERRUPT_TEST] = 1;
		}

		tg3_full_lock(tp, 0);

		tg3_halt(tp, RESET_KIND_SHUTDOWN, 1);
		if (netif_running(dev)) {
			tg3_flag_set(tp, INIT_COMPLETE);
			err2 = tg3_restart_hw(tp, 1);
			if (!err2)
				tg3_netif_start(tp);
		}

		tg3_full_unlock(tp);

		if (irq_sync && !err2)
			tg3_phy_start(tp);
	}
	if (tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER)
		tg3_power_down(tp);

}
