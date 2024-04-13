static int cma_netdev_callback(struct notifier_block *self, unsigned long event,
			       void *ptr)
{
	struct net_device *ndev = netdev_notifier_info_to_dev(ptr);
	struct cma_device *cma_dev;
	struct rdma_id_private *id_priv;
	int ret = NOTIFY_DONE;

	if (dev_net(ndev) != &init_net)
		return NOTIFY_DONE;

	if (event != NETDEV_BONDING_FAILOVER)
		return NOTIFY_DONE;

	if (!(ndev->flags & IFF_MASTER) || !(ndev->priv_flags & IFF_BONDING))
		return NOTIFY_DONE;

	mutex_lock(&lock);
	list_for_each_entry(cma_dev, &dev_list, list)
		list_for_each_entry(id_priv, &cma_dev->id_list, list) {
			ret = cma_netdev_change(ndev, id_priv);
			if (ret)
				goto out;
		}

out:
	mutex_unlock(&lock);
	return ret;
}
