static int edge_port_probe(struct usb_serial_port *port)
{
	struct edgeport_port *edge_port;
	int ret;

	edge_port = kzalloc(sizeof(*edge_port), GFP_KERNEL);
	if (!edge_port)
		return -ENOMEM;

	spin_lock_init(&edge_port->ep_lock);
	edge_port->port = port;
	edge_port->edge_serial = usb_get_serial_data(port->serial);
	edge_port->bUartMode = default_uart_mode;

	switch (port->port_number) {
	case 0:
		edge_port->uart_base = UMPMEM_BASE_UART1;
		edge_port->dma_address = UMPD_OEDB1_ADDRESS;
		break;
	case 1:
		edge_port->uart_base = UMPMEM_BASE_UART2;
		edge_port->dma_address = UMPD_OEDB2_ADDRESS;
		break;
	default:
		dev_err(&port->dev, "unknown port number\n");
		ret = -ENODEV;
		goto err;
	}

	dev_dbg(&port->dev,
		"%s - port_number = %d, uart_base = %04x, dma_address = %04x\n",
		__func__, port->port_number, edge_port->uart_base,
		edge_port->dma_address);

	usb_set_serial_port_data(port, edge_port);

	ret = edge_create_sysfs_attrs(port);
	if (ret)
		goto err;

	port->port.closing_wait = msecs_to_jiffies(closing_wait * 10);
	port->port.drain_delay = 1;

	return 0;
err:
	kfree(edge_port);

	return ret;
}
