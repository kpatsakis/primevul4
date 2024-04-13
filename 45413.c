static struct atalk_iface *atif_add_device(struct net_device *dev,
					   struct atalk_addr *sa)
{
	struct atalk_iface *iface = kzalloc(sizeof(*iface), GFP_KERNEL);

	if (!iface)
		goto out;

	dev_hold(dev);
	iface->dev = dev;
	dev->atalk_ptr = iface;
	iface->address = *sa;
	iface->status = 0;

	write_lock_bh(&atalk_interfaces_lock);
	iface->next = atalk_interfaces;
	atalk_interfaces = iface;
	write_unlock_bh(&atalk_interfaces_lock);
out:
	return iface;
}
