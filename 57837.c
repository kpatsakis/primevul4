static int edge_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	struct edgeport_serial *edge_serial;
	struct usb_device *dev;
	struct urb *urb;
	int port_number;
	int status;
	u16 open_settings;
	u8 transaction_timeout;

	if (edge_port == NULL)
		return -ENODEV;

	port_number = port->port_number;

	dev = port->serial->dev;

	/* turn off loopback */
	status = ti_do_config(edge_port, UMPC_SET_CLR_LOOPBACK, 0);
	if (status) {
		dev_err(&port->dev,
				"%s - cannot send clear loopback command, %d\n",
			__func__, status);
		return status;
	}

	/* set up the port settings */
	if (tty)
		edge_set_termios(tty, port, &tty->termios);

	/* open up the port */

	/* milliseconds to timeout for DMA transfer */
	transaction_timeout = 2;

	edge_port->ump_read_timeout =
				max(20, ((transaction_timeout * 3) / 2));

	/* milliseconds to timeout for DMA transfer */
	open_settings = (u8)(UMP_DMA_MODE_CONTINOUS |
			     UMP_PIPE_TRANS_TIMEOUT_ENA |
			     (transaction_timeout << 2));

	dev_dbg(&port->dev, "%s - Sending UMPC_OPEN_PORT\n", __func__);

	/* Tell TI to open and start the port */
	status = send_cmd(dev, UMPC_OPEN_PORT,
		(u8)(UMPM_UART1_PORT + port_number), open_settings, NULL, 0);
	if (status) {
		dev_err(&port->dev, "%s - cannot send open command, %d\n",
							__func__, status);
		return status;
	}

	/* Start the DMA? */
	status = send_cmd(dev, UMPC_START_PORT,
		(u8)(UMPM_UART1_PORT + port_number), 0, NULL, 0);
	if (status) {
		dev_err(&port->dev, "%s - cannot send start DMA command, %d\n",
							__func__, status);
		return status;
	}

	/* Clear TX and RX buffers in UMP */
	status = purge_port(port, UMP_PORT_DIR_OUT | UMP_PORT_DIR_IN);
	if (status) {
		dev_err(&port->dev,
			"%s - cannot send clear buffers command, %d\n",
			__func__, status);
		return status;
	}

	/* Read Initial MSR */
	status = ti_vread_sync(dev, UMPC_READ_MSR, 0,
				(__u16)(UMPM_UART1_PORT + port_number),
				&edge_port->shadow_msr, 1);
	if (status) {
		dev_err(&port->dev, "%s - cannot send read MSR command, %d\n",
							__func__, status);
		return status;
	}

	dev_dbg(&port->dev, "ShadowMSR 0x%X\n", edge_port->shadow_msr);

	/* Set Initial MCR */
	edge_port->shadow_mcr = MCR_RTS | MCR_DTR;
	dev_dbg(&port->dev, "ShadowMCR 0x%X\n", edge_port->shadow_mcr);

	edge_serial = edge_port->edge_serial;
	if (mutex_lock_interruptible(&edge_serial->es_lock))
		return -ERESTARTSYS;
	if (edge_serial->num_ports_open == 0) {
		/* we are the first port to open, post the interrupt urb */
		urb = edge_serial->serial->port[0]->interrupt_in_urb;
		if (!urb) {
			dev_err(&port->dev,
				"%s - no interrupt urb present, exiting\n",
				__func__);
			status = -EINVAL;
			goto release_es_lock;
		}
		urb->context = edge_serial;
		status = usb_submit_urb(urb, GFP_KERNEL);
		if (status) {
			dev_err(&port->dev,
				"%s - usb_submit_urb failed with value %d\n",
					__func__, status);
			goto release_es_lock;
		}
	}

	/*
	 * reset the data toggle on the bulk endpoints to work around bug in
	 * host controllers where things get out of sync some times
	 */
	usb_clear_halt(dev, port->write_urb->pipe);
	usb_clear_halt(dev, port->read_urb->pipe);

	/* start up our bulk read urb */
	urb = port->read_urb;
	if (!urb) {
		dev_err(&port->dev, "%s - no read urb present, exiting\n",
								__func__);
		status = -EINVAL;
		goto unlink_int_urb;
	}
	edge_port->ep_read_urb_state = EDGE_READ_URB_RUNNING;
	urb->context = edge_port;
	status = usb_submit_urb(urb, GFP_KERNEL);
	if (status) {
		dev_err(&port->dev,
			"%s - read bulk usb_submit_urb failed with value %d\n",
				__func__, status);
		goto unlink_int_urb;
	}

	++edge_serial->num_ports_open;

	goto release_es_lock;

unlink_int_urb:
	if (edge_port->edge_serial->num_ports_open == 0)
		usb_kill_urb(port->serial->port[0]->interrupt_in_urb);
release_es_lock:
	mutex_unlock(&edge_serial->es_lock);
	return status;
}
