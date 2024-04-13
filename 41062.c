static void host_stop(struct ci13xxx *ci)
{
	struct usb_hcd *hcd = ci->hcd;

	usb_remove_hcd(hcd);
	usb_put_hcd(hcd);
}
