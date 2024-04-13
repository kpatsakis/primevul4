static int compat_x25_subscr_ioctl(unsigned int cmd,
		struct compat_x25_subscrip_struct __user *x25_subscr32)
{
	struct compat_x25_subscrip_struct x25_subscr;
	struct x25_neigh *nb;
	struct net_device *dev;
	int rc = -EINVAL;

	rc = -EFAULT;
	if (copy_from_user(&x25_subscr, x25_subscr32, sizeof(*x25_subscr32)))
		goto out;

	rc = -EINVAL;
	dev = x25_dev_get(x25_subscr.device);
	if (dev == NULL)
		goto out;

	nb = x25_get_neigh(dev);
	if (nb == NULL)
		goto out_dev_put;

	dev_put(dev);

	if (cmd == SIOCX25GSUBSCRIP) {
		read_lock_bh(&x25_neigh_list_lock);
		x25_subscr.extended = nb->extended;
		x25_subscr.global_facil_mask = nb->global_facil_mask;
		read_unlock_bh(&x25_neigh_list_lock);
		rc = copy_to_user(x25_subscr32, &x25_subscr,
				sizeof(*x25_subscr32)) ? -EFAULT : 0;
	} else {
		rc = -EINVAL;
		if (x25_subscr.extended == 0 || x25_subscr.extended == 1) {
			rc = 0;
			write_lock_bh(&x25_neigh_list_lock);
			nb->extended = x25_subscr.extended;
			nb->global_facil_mask = x25_subscr.global_facil_mask;
			write_unlock_bh(&x25_neigh_list_lock);
		}
	}
	x25_neigh_put(nb);
out:
	return rc;
out_dev_put:
	dev_put(dev);
	goto out;
}
