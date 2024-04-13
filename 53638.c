int usb_unlocked_disable_lpm(struct usb_device *udev)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);
	int ret;

	if (!hcd)
		return -EINVAL;

	mutex_lock(hcd->bandwidth_mutex);
	ret = usb_disable_lpm(udev);
	mutex_unlock(hcd->bandwidth_mutex);

	return ret;
}
