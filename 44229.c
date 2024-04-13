static void cm_remove_one(struct ib_device *ib_device)
{
	struct cm_device *cm_dev;
	struct cm_port *port;
	struct ib_port_modify port_modify = {
		.clr_port_cap_mask = IB_PORT_CM_SUP
	};
	unsigned long flags;
	int i;

	cm_dev = ib_get_client_data(ib_device, &cm_client);
	if (!cm_dev)
		return;

	write_lock_irqsave(&cm.device_lock, flags);
	list_del(&cm_dev->list);
	write_unlock_irqrestore(&cm.device_lock, flags);

	for (i = 1; i <= ib_device->phys_port_cnt; i++) {
		port = cm_dev->port[i-1];
		ib_modify_port(ib_device, port->port_num, 0, &port_modify);
		ib_unregister_mad_agent(port->mad_agent);
		flush_workqueue(cm.wq);
		cm_remove_port_fs(port);
	}
	device_unregister(cm_dev->device);
	kfree(cm_dev);
}
