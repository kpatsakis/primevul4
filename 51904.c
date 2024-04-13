static int ppp_dev_init(struct net_device *dev)
{
	dev->qdisc_tx_busylock = &ppp_tx_busylock;
	return 0;
}
