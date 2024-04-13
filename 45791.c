static int x25_device_event(struct notifier_block *this, unsigned long event,
			    void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct x25_neigh *nb;

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	if (dev->type == ARPHRD_X25
#if IS_ENABLED(CONFIG_LLC)
	 || dev->type == ARPHRD_ETHER
#endif
	 ) {
		switch (event) {
		case NETDEV_UP:
			x25_link_device_up(dev);
			break;
		case NETDEV_GOING_DOWN:
			nb = x25_get_neigh(dev);
			if (nb) {
				x25_terminate_link(nb);
				x25_neigh_put(nb);
			}
			break;
		case NETDEV_DOWN:
			x25_kill_by_device(dev);
			x25_route_device_down(dev);
			x25_link_device_down(dev);
			break;
		}
	}

	return NOTIFY_DONE;
}
