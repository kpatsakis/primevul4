static int whiteheat_firmware_attach(struct usb_serial *serial)
{
	/* We want this device to fail to have a driver assigned to it */
	return 1;
}
