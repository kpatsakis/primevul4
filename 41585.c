static void free_urbs(struct wdm_device *desc)
{
	usb_free_urb(desc->validity);
	usb_free_urb(desc->response);
	usb_free_urb(desc->command);
}
