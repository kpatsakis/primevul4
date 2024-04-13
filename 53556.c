static void hub_free_dev(struct usb_device *udev)
{
	struct usb_hcd *hcd = bus_to_hcd(udev->bus);

	/* Root hubs aren't real devices, so don't free HCD resources */
	if (hcd->driver->free_dev && udev->parent)
		hcd->driver->free_dev(hcd, udev);
}
