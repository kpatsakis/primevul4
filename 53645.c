static unsigned wakeup_enabled_descendants(struct usb_device *udev)
{
	struct usb_hub *hub = usb_hub_to_struct_hub(udev);

	return udev->do_remote_wakeup +
			(hub ? hub->wakeup_enabled_descendants : 0);
}
