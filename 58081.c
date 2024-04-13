static struct port *find_port_by_id(struct ports_device *portdev, u32 id)
{
	struct port *port;
	unsigned long flags;

	spin_lock_irqsave(&portdev->ports_lock, flags);
	list_for_each_entry(port, &portdev->ports, list)
		if (port->id == id)
			goto out;
	port = NULL;
out:
	spin_unlock_irqrestore(&portdev->ports_lock, flags);

	return port;
}
