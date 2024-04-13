static void ppp_dev_uninit(struct net_device *dev)
{
	struct ppp *ppp = netdev_priv(dev);
	struct ppp_net *pn = ppp_pernet(ppp->ppp_net);

	ppp_lock(ppp);
	ppp->closing = 1;
	ppp_unlock(ppp);

	mutex_lock(&pn->all_ppp_mutex);
	unit_put(&pn->units_idr, ppp->file.index);
	mutex_unlock(&pn->all_ppp_mutex);

	ppp->owner = NULL;

	ppp->file.dead = 1;
	wake_up_interruptible(&ppp->file.rwait);
}
