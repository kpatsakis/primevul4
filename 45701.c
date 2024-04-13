static int rose_device_event(struct notifier_block *this,
			     unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	if (event != NETDEV_DOWN)
		return NOTIFY_DONE;

	switch (dev->type) {
	case ARPHRD_ROSE:
		rose_kill_by_device(dev);
		break;
	case ARPHRD_AX25:
		rose_link_device_down(dev);
		rose_rt_device_down(dev);
		break;
	}

	return NOTIFY_DONE;
}
