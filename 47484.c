static int pndisc_is_router(const void *pkey,
			    struct net_device *dev)
{
	struct pneigh_entry *n;
	int ret = -1;

	read_lock_bh(&nd_tbl.lock);
	n = __pneigh_lookup(&nd_tbl, dev_net(dev), pkey, dev);
	if (n)
		ret = !!(n->flags & NTF_ROUTER);
	read_unlock_bh(&nd_tbl.lock);

	return ret;
}
