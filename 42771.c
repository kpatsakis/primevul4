struct net_device *alloc_netdev_mqs(int sizeof_priv, const char *name,
		void (*setup)(struct net_device *),
		unsigned int txqs, unsigned int rxqs)
{
	struct net_device *dev;
	size_t alloc_size;
	struct net_device *p;

	BUG_ON(strlen(name) >= sizeof(dev->name));

	if (txqs < 1) {
		pr_err("alloc_netdev: Unable to allocate device "
		       "with zero queues.\n");
		return NULL;
	}

#ifdef CONFIG_RPS
	if (rxqs < 1) {
		pr_err("alloc_netdev: Unable to allocate device "
		       "with zero RX queues.\n");
		return NULL;
	}
#endif

	alloc_size = sizeof(struct net_device);
	if (sizeof_priv) {
		/* ensure 32-byte alignment of private area */
		alloc_size = ALIGN(alloc_size, NETDEV_ALIGN);
		alloc_size += sizeof_priv;
	}
	/* ensure 32-byte alignment of whole construct */
	alloc_size += NETDEV_ALIGN - 1;

	p = kzalloc(alloc_size, GFP_KERNEL);
	if (!p) {
		printk(KERN_ERR "alloc_netdev: Unable to allocate device.\n");
		return NULL;
	}

	dev = PTR_ALIGN(p, NETDEV_ALIGN);
	dev->padded = (char *)dev - (char *)p;

	dev->pcpu_refcnt = alloc_percpu(int);
	if (!dev->pcpu_refcnt)
		goto free_p;

	if (dev_addr_init(dev))
		goto free_pcpu;

	dev_mc_init(dev);
	dev_uc_init(dev);

	dev_net_set(dev, &init_net);

	dev->gso_max_size = GSO_MAX_SIZE;

	INIT_LIST_HEAD(&dev->ethtool_ntuple_list.list);
	dev->ethtool_ntuple_list.count = 0;
	INIT_LIST_HEAD(&dev->napi_list);
	INIT_LIST_HEAD(&dev->unreg_list);
	INIT_LIST_HEAD(&dev->link_watch_list);
	dev->priv_flags = IFF_XMIT_DST_RELEASE;
	setup(dev);

	dev->num_tx_queues = txqs;
	dev->real_num_tx_queues = txqs;
	if (netif_alloc_netdev_queues(dev))
		goto free_all;

#ifdef CONFIG_RPS
	dev->num_rx_queues = rxqs;
	dev->real_num_rx_queues = rxqs;
	if (netif_alloc_rx_queues(dev))
		goto free_all;
#endif

	strcpy(dev->name, name);
	return dev;

free_all:
	free_netdev(dev);
	return NULL;

free_pcpu:
	free_percpu(dev->pcpu_refcnt);
	kfree(dev->_tx);
#ifdef CONFIG_RPS
	kfree(dev->_rx);
#endif

free_p:
	kfree(p);
	return NULL;
}
