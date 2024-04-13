static void __usb_queue_reset_device(struct work_struct *ws)
{
	int rc;
	struct usb_interface *iface =
		container_of(ws, struct usb_interface, reset_ws);
	struct usb_device *udev = interface_to_usbdev(iface);

	rc = usb_lock_device_for_reset(udev, iface);
	if (rc >= 0) {
		usb_reset_device(udev);
		usb_unlock_device(udev);
	}
	usb_put_intf(iface);	/* Undo _get_ in usb_queue_reset_device() */
}
