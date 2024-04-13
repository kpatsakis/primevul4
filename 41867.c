static void edge_close(struct usb_serial_port *port)
{
	struct edgeport_serial *edge_serial;
	struct edgeport_port *edge_port;
	struct usb_serial *serial = port->serial;
	int port_number;

	edge_serial = usb_get_serial_data(port->serial);
	edge_port = usb_get_serial_port_data(port);
	if (edge_serial == NULL || edge_port == NULL)
		return;

	/* The bulkreadcompletion routine will check
	 * this flag and dump add read data */
	edge_port->close_pending = 1;

	/* chase the port close and flush */
	chase_port(edge_port, (HZ * closing_wait) / 100, 1);

	usb_kill_urb(port->read_urb);
	usb_kill_urb(port->write_urb);
	edge_port->ep_write_urb_in_use = 0;

	/* assuming we can still talk to the device,
	 * send a close port command to it */
	dev_dbg(&port->dev, "%s - send umpc_close_port\n", __func__);
	port_number = port->number - port->serial->minor;

	mutex_lock(&serial->disc_mutex);
	if (!serial->disconnected) {
		send_cmd(serial->dev,
				     UMPC_CLOSE_PORT,
				     (__u8)(UMPM_UART1_PORT + port_number),
				     0,
				     NULL,
				     0);
	}
	mutex_unlock(&serial->disc_mutex);

	mutex_lock(&edge_serial->es_lock);
	--edge_port->edge_serial->num_ports_open;
	if (edge_port->edge_serial->num_ports_open <= 0) {
		/* last port is now closed, let's shut down our interrupt urb */
		usb_kill_urb(port->serial->port[0]->interrupt_in_urb);
		edge_port->edge_serial->num_ports_open = 0;
	}
	mutex_unlock(&edge_serial->es_lock);
	edge_port->close_pending = 0;
}
