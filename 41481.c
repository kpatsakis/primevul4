static void tg3_remove_one(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);

	if (dev) {
		struct tg3 *tp = netdev_priv(dev);

		release_firmware(tp->fw);

		tg3_reset_task_cancel(tp);

		if (tg3_flag(tp, USE_PHYLIB)) {
			tg3_phy_fini(tp);
			tg3_mdio_fini(tp);
		}

		unregister_netdev(dev);
		if (tp->aperegs) {
			iounmap(tp->aperegs);
			tp->aperegs = NULL;
		}
		if (tp->regs) {
			iounmap(tp->regs);
			tp->regs = NULL;
		}
		free_netdev(dev);
		pci_release_regions(pdev);
		pci_disable_device(pdev);
		pci_set_drvdata(pdev, NULL);
	}
}
