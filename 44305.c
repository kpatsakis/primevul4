static int cma_netdev_change(struct net_device *ndev, struct rdma_id_private *id_priv)
{
	struct rdma_dev_addr *dev_addr;
	struct cma_ndev_work *work;

	dev_addr = &id_priv->id.route.addr.dev_addr;

	if ((dev_addr->bound_dev_if == ndev->ifindex) &&
	    memcmp(dev_addr->src_dev_addr, ndev->dev_addr, ndev->addr_len)) {
		printk(KERN_INFO "RDMA CM addr change for ndev %s used by id %p\n",
		       ndev->name, &id_priv->id);
		work = kzalloc(sizeof *work, GFP_KERNEL);
		if (!work)
			return -ENOMEM;

		INIT_WORK(&work->work, cma_ndev_work_handler);
		work->id = id_priv;
		work->event.event = RDMA_CM_EVENT_ADDR_CHANGE;
		atomic_inc(&id_priv->refcount);
		queue_work(cma_wq, &work->work);
	}

	return 0;
}
