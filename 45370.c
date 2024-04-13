static int pppoe_device_event(struct notifier_block *this,
			      unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	/* Only look at sockets that are using this specific device. */
	switch (event) {
	case NETDEV_CHANGEADDR:
	case NETDEV_CHANGEMTU:
		/* A change in mtu or address is a bad thing, requiring
		 * LCP re-negotiation.
		 */

	case NETDEV_GOING_DOWN:
	case NETDEV_DOWN:
		/* Find every socket on this device and kill it. */
		pppoe_flush_dev(dev);
		break;

	default:
		break;
	}

	return NOTIFY_DONE;
}
