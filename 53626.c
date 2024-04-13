int usb_remote_wakeup(struct usb_device *udev)
{
	int	status = 0;

	usb_lock_device(udev);
	if (udev->state == USB_STATE_SUSPENDED) {
		dev_dbg(&udev->dev, "usb %sresume\n", "wakeup-");
		status = usb_autoresume_device(udev);
		if (status == 0) {
			/* Let the drivers do their thing, then... */
			usb_autosuspend_device(udev);
		}
	}
	usb_unlock_device(udev);
	return status;
}
