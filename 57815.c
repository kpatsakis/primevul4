static void omninet_disconnect(struct usb_serial *serial)
{
	struct usb_serial_port *wport = serial->port[1];

	usb_kill_urb(wport->write_urb);
}
