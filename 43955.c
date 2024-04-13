static int whiteheat_ioctl(struct tty_struct *tty,
					unsigned int cmd, unsigned long arg)
{
	struct usb_serial_port *port = tty->driver_data;
	struct serial_struct serstruct;
	void __user *user_arg = (void __user *)arg;

	switch (cmd) {
	case TIOCGSERIAL:
		memset(&serstruct, 0, sizeof(serstruct));
		serstruct.type = PORT_16654;
		serstruct.line = port->minor;
		serstruct.port = port->port_number;
		serstruct.flags = ASYNC_SKIP_TEST | ASYNC_AUTO_IRQ;
		serstruct.xmit_fifo_size = kfifo_size(&port->write_fifo);
		serstruct.custom_divisor = 0;
		serstruct.baud_base = 460800;
		serstruct.close_delay = CLOSING_DELAY;
		serstruct.closing_wait = CLOSING_DELAY;

		if (copy_to_user(user_arg, &serstruct, sizeof(serstruct)))
			return -EFAULT;
		break;
	default:
		break;
	}

	return -ENOIOCTLCMD;
}
