static void kill_urbs(struct wdm_device *desc)
{
	/* the order here is essential */
	usb_kill_urb(desc->command);
	usb_kill_urb(desc->validity);
	usb_kill_urb(desc->response);
}
