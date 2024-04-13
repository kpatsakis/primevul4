static void tg3_io_resume(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct tg3 *tp = netdev_priv(netdev);
	int err;

	rtnl_lock();

	if (!netif_running(netdev))
		goto done;

	tg3_full_lock(tp, 0);
	tg3_flag_set(tp, INIT_COMPLETE);
	err = tg3_restart_hw(tp, 1);
	if (err) {
		tg3_full_unlock(tp);
		netdev_err(netdev, "Cannot restart hardware after reset.\n");
		goto done;
	}

	netif_device_attach(netdev);

	tg3_timer_start(tp);

	tg3_netif_start(tp);

	tg3_full_unlock(tp);

	tg3_phy_start(tp);

done:
	rtnl_unlock();
}
