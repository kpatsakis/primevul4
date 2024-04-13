static int edge_ioctl(struct tty_struct *tty,
					unsigned int cmd, unsigned long arg)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);

	switch (cmd) {
	case TIOCGSERIAL:
		dev_dbg(&port->dev, "%s - TIOCGSERIAL\n", __func__);
		return get_serial_info(edge_port,
				(struct serial_struct __user *) arg);
	}
	return -ENOIOCTLCMD;
}
