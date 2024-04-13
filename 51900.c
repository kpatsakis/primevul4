static struct ppp *ppp_create_interface(struct net *net, int unit,
					struct file *file, int *retp)
{
	struct ppp *ppp;
	struct ppp_net *pn;
	struct net_device *dev = NULL;
	int ret = -ENOMEM;
	int i;

	dev = alloc_netdev(sizeof(struct ppp), "", NET_NAME_ENUM, ppp_setup);
	if (!dev)
		goto out1;

	pn = ppp_pernet(net);

	ppp = netdev_priv(dev);
	ppp->dev = dev;
	ppp->mru = PPP_MRU;
	init_ppp_file(&ppp->file, INTERFACE);
	ppp->file.hdrlen = PPP_HDRLEN - 2;	/* don't count proto bytes */
	ppp->owner = file;
	for (i = 0; i < NUM_NP; ++i)
		ppp->npmode[i] = NPMODE_PASS;
	INIT_LIST_HEAD(&ppp->channels);
	spin_lock_init(&ppp->rlock);
	spin_lock_init(&ppp->wlock);
#ifdef CONFIG_PPP_MULTILINK
	ppp->minseq = -1;
	skb_queue_head_init(&ppp->mrq);
#endif /* CONFIG_PPP_MULTILINK */
#ifdef CONFIG_PPP_FILTER
	ppp->pass_filter = NULL;
	ppp->active_filter = NULL;
#endif /* CONFIG_PPP_FILTER */

	/*
	 * drum roll: don't forget to set
	 * the net device is belong to
	 */
	dev_net_set(dev, net);

	rtnl_lock();
	mutex_lock(&pn->all_ppp_mutex);

	if (unit < 0) {
		unit = unit_get(&pn->units_idr, ppp);
		if (unit < 0) {
			ret = unit;
			goto out2;
		}
	} else {
		ret = -EEXIST;
		if (unit_find(&pn->units_idr, unit))
			goto out2; /* unit already exists */
		/*
		 * if caller need a specified unit number
		 * lets try to satisfy him, otherwise --
		 * he should better ask us for new unit number
		 *
		 * NOTE: yes I know that returning EEXIST it's not
		 * fair but at least pppd will ask us to allocate
		 * new unit in this case so user is happy :)
		 */
		unit = unit_set(&pn->units_idr, ppp, unit);
		if (unit < 0)
			goto out2;
	}

	/* Initialize the new ppp unit */
	ppp->file.index = unit;
	sprintf(dev->name, "ppp%d", unit);

	ret = register_netdevice(dev);
	if (ret != 0) {
		unit_put(&pn->units_idr, unit);
		netdev_err(ppp->dev, "PPP: couldn't register device %s (%d)\n",
			   dev->name, ret);
		goto out2;
	}

	ppp->ppp_net = net;

	atomic_inc(&ppp_unit_count);
	mutex_unlock(&pn->all_ppp_mutex);
	rtnl_unlock();

	*retp = 0;
	return ppp;

out2:
	mutex_unlock(&pn->all_ppp_mutex);
	rtnl_unlock();
	free_netdev(dev);
out1:
	*retp = ret;
	return NULL;
}
