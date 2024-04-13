static void update_devnum(struct usb_device *udev, int devnum)
{
	/* The address for a WUSB device is managed by wusbcore. */
	if (!udev->wusb)
		udev->devnum = devnum;
}
