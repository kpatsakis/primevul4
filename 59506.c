static void klsi_105_close(struct usb_serial_port *port)
{
	int rc;

	/* send READ_OFF */
	rc = usb_control_msg(port->serial->dev,
			     usb_sndctrlpipe(port->serial->dev, 0),
			     KL5KUSB105A_SIO_CONFIGURE,
			     USB_TYPE_VENDOR | USB_DIR_OUT,
			     KL5KUSB105A_SIO_CONFIGURE_READ_OFF,
			     0, /* index */
			     NULL, 0,
			     KLSI_TIMEOUT);
	if (rc < 0)
		dev_err(&port->dev, "failed to disable read: %d\n", rc);

	/* shutdown our bulk reads and writes */
	usb_serial_generic_close(port);
}
