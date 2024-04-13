void usb_ep0_reinit(struct usb_device *udev)
{
	usb_disable_endpoint(udev, 0 + USB_DIR_IN, true);
	usb_disable_endpoint(udev, 0 + USB_DIR_OUT, true);
	usb_enable_endpoint(udev, &udev->ep0, true);
}
