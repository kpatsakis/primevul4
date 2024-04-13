fst_open(struct net_device *dev)
{
	int err;
	struct fst_port_info *port;

	port = dev_to_port(dev);
	if (!try_module_get(THIS_MODULE))
          return -EBUSY;

	if (port->mode != FST_RAW) {
		err = hdlc_open(dev);
		if (err) {
			module_put(THIS_MODULE);
			return err;
		}
	}

	fst_openport(port);
	netif_wake_queue(dev);
	return 0;
}
