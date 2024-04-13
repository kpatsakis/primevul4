static int ax25_device_event(struct notifier_block *this, unsigned long event,
			     void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	/* Reject non AX.25 devices */
	if (dev->type != ARPHRD_AX25)
		return NOTIFY_DONE;

	switch (event) {
	case NETDEV_UP:
		ax25_dev_device_up(dev);
		break;
	case NETDEV_DOWN:
		ax25_kill_by_device(dev);
		ax25_rt_device_down(dev);
		ax25_dev_device_down(dev);
		break;
	default:
		break;
	}

	return NOTIFY_DONE;
}
