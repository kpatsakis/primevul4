static struct port *find_port_by_devt_in_portdev(struct ports_device *portdev,
						 dev_t dev)
{
	struct port *port;
	unsigned long flags;

	spin_lock_irqsave(&portdev->ports_lock, flags);
	list_for_each_entry(port, &portdev->ports, list) {
		if (port->cdev->dev == dev) {
			kref_get(&port->kref);
			goto out;
		}
	}
	port = NULL;
out:
	spin_unlock_irqrestore(&portdev->ports_lock, flags);

	return port;
}
