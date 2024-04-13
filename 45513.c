static int ipxitf_delete(struct ipx_interface_definition *idef)
{
	struct net_device *dev = NULL;
	__be16 dlink_type = 0;
	struct ipx_interface *intrfc;
	int rc = 0;

	spin_lock_bh(&ipx_interfaces_lock);
	if (idef->ipx_special == IPX_INTERNAL) {
		if (ipx_internal_net) {
			__ipxitf_put(ipx_internal_net);
			goto out;
		}
		rc = -ENOENT;
		goto out;
	}

	dlink_type = ipx_map_frame_type(idef->ipx_dlink_type);
	rc = -EPROTONOSUPPORT;
	if (!dlink_type)
		goto out;

	dev = __dev_get_by_name(&init_net, idef->ipx_device);
	rc = -ENODEV;
	if (!dev)
		goto out;

	intrfc = __ipxitf_find_using_phys(dev, dlink_type);
	rc = -EINVAL;
	if (!intrfc)
		goto out;
	__ipxitf_put(intrfc);

	rc = 0;
out:
	spin_unlock_bh(&ipx_interfaces_lock);
	return rc;
}
