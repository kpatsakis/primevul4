static int ndisc_netdev_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct net *net = dev_net(dev);
	struct inet6_dev *idev;

	switch (event) {
	case NETDEV_CHANGEADDR:
		neigh_changeaddr(&nd_tbl, dev);
		fib6_run_gc(0, net, false);
		idev = in6_dev_get(dev);
		if (!idev)
			break;
		if (idev->cnf.ndisc_notify)
			ndisc_send_unsol_na(dev);
		in6_dev_put(idev);
		break;
	case NETDEV_DOWN:
		neigh_ifdown(&nd_tbl, dev);
		fib6_run_gc(0, net, false);
		break;
	case NETDEV_NOTIFY_PEERS:
		ndisc_send_unsol_na(dev);
		break;
	default:
		break;
	}

	return NOTIFY_DONE;
}
