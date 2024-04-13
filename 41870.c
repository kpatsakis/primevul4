static void edge_interrupt_callback(struct urb *urb)
{
	struct edgeport_serial *edge_serial = urb->context;
	struct usb_serial_port *port;
	struct edgeport_port *edge_port;
	struct device *dev;
	unsigned char *data = urb->transfer_buffer;
	int length = urb->actual_length;
	int port_number;
	int function;
	int retval;
	__u8 lsr;
	__u8 msr;
	int status = urb->status;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(&urb->dev->dev, "%s - urb shutting down with status: %d\n",
		    __func__, status);
		return;
	default:
		dev_err(&urb->dev->dev, "%s - nonzero urb status received: "
			"%d\n", __func__, status);
		goto exit;
	}

	if (!length) {
		dev_dbg(&urb->dev->dev, "%s - no data in urb\n", __func__);
		goto exit;
	}

	dev = &edge_serial->serial->dev->dev;
	usb_serial_debug_data(dev, __func__, length, data);

	if (length != 2) {
		dev_dbg(dev, "%s - expecting packet of size 2, got %d\n", __func__, length);
		goto exit;
	}

	port_number = TIUMP_GET_PORT_FROM_CODE(data[0]);
	function    = TIUMP_GET_FUNC_FROM_CODE(data[0]);
	dev_dbg(dev, "%s - port_number %d, function %d, info 0x%x\n", __func__,
		port_number, function, data[1]);
	port = edge_serial->serial->port[port_number];
	edge_port = usb_get_serial_port_data(port);
	if (!edge_port) {
		dev_dbg(dev, "%s - edge_port not found\n", __func__);
		return;
	}
	switch (function) {
	case TIUMP_INTERRUPT_CODE_LSR:
		lsr = map_line_status(data[1]);
		if (lsr & UMP_UART_LSR_DATA_MASK) {
			/* Save the LSR event for bulk read
			   completion routine */
			dev_dbg(dev, "%s - LSR Event Port %u LSR Status = %02x\n",
				__func__, port_number, lsr);
			edge_port->lsr_event = 1;
			edge_port->lsr_mask = lsr;
		} else {
			dev_dbg(dev, "%s - ===== Port %d LSR Status = %02x ======\n",
				__func__, port_number, lsr);
			handle_new_lsr(edge_port, 0, lsr, 0);
		}
		break;

	case TIUMP_INTERRUPT_CODE_MSR:	/* MSR */
		/* Copy MSR from UMP */
		msr = data[1];
		dev_dbg(dev, "%s - ===== Port %u MSR Status = %02x ======\n",
			__func__, port_number, msr);
		handle_new_msr(edge_port, msr);
		break;

	default:
		dev_err(&urb->dev->dev,
			"%s - Unknown Interrupt code from UMP %x\n",
			__func__, data[1]);
		break;

	}

exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval)
		dev_err(&urb->dev->dev,
			"%s - usb_submit_urb failed with result %d\n",
			 __func__, retval);
}
