static int usbdev_notify(struct notifier_block *self,
			       unsigned long action, void *dev)
{
	switch (action) {
	case USB_DEVICE_ADD:
		break;
	case USB_DEVICE_REMOVE:
		usbdev_remove(dev);
		break;
	}
	return NOTIFY_OK;
}
