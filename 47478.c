static void ndisc_send_unsol_na(struct net_device *dev)
{
	struct inet6_dev *idev;
	struct inet6_ifaddr *ifa;

	idev = in6_dev_get(dev);
	if (!idev)
		return;

	read_lock_bh(&idev->lock);
	list_for_each_entry(ifa, &idev->addr_list, if_list) {
		ndisc_send_na(dev, NULL, &in6addr_linklocal_allnodes, &ifa->addr,
			      /*router=*/ !!idev->cnf.forwarding,
			      /*solicited=*/ false, /*override=*/ true,
			      /*inc_opt=*/ true);
	}
	read_unlock_bh(&idev->lock);

	in6_dev_put(idev);
}
