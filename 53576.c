static void hub_release(struct kref *kref)
{
	struct usb_hub *hub = container_of(kref, struct usb_hub, kref);

	usb_put_dev(hub->hdev);
	usb_put_intf(to_usb_interface(hub->intfdev));
	kfree(hub);
}
