static struct port *find_port_by_devt(dev_t dev)
{
	struct ports_device *portdev;
	struct port *port;
	unsigned long flags;

	spin_lock_irqsave(&pdrvdata_lock, flags);
	list_for_each_entry(portdev, &pdrvdata.portdevs, list) {
		port = find_port_by_devt_in_portdev(portdev, dev);
		if (port)
			goto out;
	}
	port = NULL;
out:
	spin_unlock_irqrestore(&pdrvdata_lock, flags);
	return port;
}
