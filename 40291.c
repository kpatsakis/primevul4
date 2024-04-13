int rtnl_configure_link(struct net_device *dev, const struct ifinfomsg *ifm)
{
	unsigned int old_flags;
	int err;

	old_flags = dev->flags;
	if (ifm && (ifm->ifi_flags || ifm->ifi_change)) {
		err = __dev_change_flags(dev, rtnl_dev_combine_flags(dev, ifm));
		if (err < 0)
			return err;
	}

	dev->rtnl_link_state = RTNL_LINK_INITIALIZED;
	rtmsg_ifinfo(RTM_NEWLINK, dev, ~0U);

	__dev_notify_flags(dev, old_flags);
	return 0;
}
